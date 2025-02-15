/*
* Copyright (c) <2021> Side Effects Software Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. The name of Side Effects Software may not be used to endorse or
*    promote products derived from this software without specific prior
*    written permission.
*
* THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
* NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "HoudiniEditorTestDataLayers.h"

#include "FileHelpers.h"
#include "HoudiniAsset.h"
#include "HoudiniPublicAPIAssetWrapper.h"
#include "HoudiniPublicAPIInputTypes.h"
#include "HoudiniAssetActor.h"
#include "HoudiniEngineEditorUtils.h"

#include "HoudiniPDGManager.h"
#include "HoudiniParameterString.h"
#include "HoudiniPDGAssetLink.h"
#include "Landscape.h"
#include "AssetRegistry/AssetRegistryModule.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "HoudiniEditorTestUtils.h"

#include "Misc/AutomationTest.h"
#include "HoudiniAssetActorFactory.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "HoudiniParameterToggle.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "HoudiniEditorUnitTestUtils.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "HoudiniEngineBakeUtils.h"

IMPLEMENT_SIMPLE_HOUDINI_AUTOMATION_TEST(FHoudiniEditorTestsDataLayers, "Houdini.UnitTests.DataLayers.PDGTest", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)


bool FHoudiniEditorTestsDataLayers::RunTest(const FString& Parameters)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// This test enusres that we can cook an HDA multiple times and the outputs are removed on each recook. The test HDA
	///	can create multiple outputs based off the parameters. By changing the parameters we can get different scenarios on
	///	the same HDA.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// Make sure we have a Houdini Session before doing anything.
	FHoudiniEditorTestUtils::CreateSessionIfInvalidWithLatentRetries(this, FHoudiniEditorTestUtils::HoudiniEngineSessionPipeName, {}, {});

	// Load the HDA into a new map and kick start the cook. We do an initial cook to make sure the parameters are available.
	static UHoudiniAssetComponent* NewHAC = FHoudiniEditorUnitTestUtils::LoadHDAIntoNewMap(TEXT("/Game/TestHDAs/PDG/PDGHarness"), FTransform::Identity, true);

	// Now create the test context. This should be the last step before the tests start as it starts the timeout timer. Note
	// the context live in a SharedPtr<> because each part of the test, in AddCommand(), are executed asyncronously
	// after the test returns.

	TSharedPtr<FHoudiniTestContext> Context(new FHoudiniTestContext(this));
	Context->HAC = NewHAC;
	Context->HAC->bOverrideGlobalProxyStaticMeshSettings = true;
	Context->HAC->bEnableProxyStaticMeshOverride = false;
	Context->StartCookingHDA();

	// HDA Path and kick PDG Cook.
	AddCommand(new FHoudiniLatentTestCommand(Context, [this, Context]()
	{
		FString HDAPath = FHoudiniEditorUnitTestUtils::GetAbsolutePathOfProjectFile(TEXT("TestHDAS/DataLayers/CreateMeshWithDataLayer.hda"));

		SET_HDA_PARAMETER(Context->HAC, UHoudiniParameterString, "hda_path", HDAPath, 0);
		Context->StartCookingSelectedTOPNetwork();
		return true;
	}));

	// Bake and check results.
	AddCommand(new FHoudiniLatentTestCommand(Context, [this, Context]()
	{
		auto & Results = Context->HAC->GetPDGAssetLink()->GetSelectedTOPNode()->WorkResult;

		// We should have one work result. Check this before baking.
		HOUDINI_TEST_EQUAL_ON_FAIL(Results.Num(), 1, return true);
		HOUDINI_TEST_EQUAL_ON_FAIL(Results[0].ResultObjects.Num(), 1, return true);

		// Bake PDG Output
		TArray<FHoudiniEngineBakedActor> BakedActors = Context->BakeSelectedTopNetwork();
		HOUDINI_TEST_EQUAL_ON_FAIL(BakedActors.Num(), 1, return true);

		TArray<UStaticMeshComponent*> StaticMeshOutputs = FHoudiniEditorUnitTestUtils::GetOutputsWithComponent<UStaticMeshComponent>(BakedActors);
		HOUDINI_TEST_EQUAL_ON_FAIL(StaticMeshOutputs.Num(), 1, return true);

		AActor * Actor = StaticMeshOutputs[0]->GetOwner();

		TArray<FHoudiniUnrealDataLayerInfo> DataLayers = FHoudiniDataLayerUtils::GetDataLayerInfoForActor(Actor);

		HOUDINI_TEST_EQUAL_ON_FAIL(DataLayers.Num(), 1, return true);
		HOUDINI_TEST_EQUAL(DataLayers[0].Name, FString("MyDataLayer"));
		return true;
	}));

	return true;
}


#endif

