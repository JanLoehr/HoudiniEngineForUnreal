﻿/*
* Copyright (c) <2023> Side Effects Software Inc.
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

#pragma once

#include "CoreMinimal.h"
#include "HoudiniEngineRuntimeCommon.h"
#include "HoudiniInput.h"
#include "HoudiniRuntimeSettings.h"
#include "HoudiniAssetComponent.h"
#include "HoudiniToolData.h"

#include "HoudiniPreset.generated.h"

/**
 * This is a Houdini Tools package descriptor inside of UE, typically created
 * after a HoudiniToolsPackage has been imported into the UE project.
 */

class UHoudiniInput;
class UHoudiniAsset;
class UHoudiniParameterChoice;
class UHoudiniParameterColor;
class UHoudiniParameterToggle;
class UHoudiniParameterInt;
class UHoudiniParameterFloat;
class UHoudiniParameterString;
class UHoudiniParameterFile;
class UHoudiniParameterRampFloat;
class UHoudiniParameterRampColor;
class UHoudiniParameterOperatorPath;


UENUM()
enum class EHoudiniPresetValueType
{
	Invalid,
	Float,
	Int,
	String
};


USTRUCT()
struct FHoudiniPresetBase
{
	GENERATED_BODY()
	virtual ~FHoudiniPresetBase() {}

	virtual FString ToString() { return FString(); }

	virtual EHoudiniPresetValueType GetValueType() { return EHoudiniPresetValueType::Invalid; }
};


USTRUCT(BlueprintType)
struct FHoudiniPresetFloatValues : public FHoudiniPresetBase
{
	GENERATED_BODY()
	
	virtual EHoudiniPresetValueType GetValueType() override { return EHoudiniPresetValueType::Float; }

	virtual FString ToString() override { return FString::JoinBy(Values, TEXT(", "), [](const float& Value) { return FString::SanitizeFloat(Value); }); }
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<float> Values;
};


USTRUCT(BlueprintType)
struct FHoudiniPresetIntValues : public FHoudiniPresetBase
{
	GENERATED_BODY()

	virtual EHoudiniPresetValueType GetValueType() override { return EHoudiniPresetValueType::Int; }

	virtual FString ToString() override { return FString::JoinBy(Values, TEXT(", "), [](const float& Value) { return FString::FromInt(Value); }); }
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<int> Values;
};


USTRUCT(BlueprintType)
struct FHoudiniPresetStringValues : public FHoudiniPresetBase
{
	GENERATED_BODY()

	virtual EHoudiniPresetValueType GetValueType() override { return EHoudiniPresetValueType::String; }

	virtual FString ToString() override { return FString::Join(Values, TEXT(", ")); }
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<FString> Values;
};

USTRUCT(BlueprintType)
struct FHoudiniPresetRampFloatPoint
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	float Position;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	float Value;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	EHoudiniRampInterpolationType Interpolation;
};

USTRUCT(BlueprintType)
struct FHoudiniPresetRampColorPoint
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	float Position;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	FLinearColor Value;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	EHoudiniRampInterpolationType Interpolation;
};

USTRUCT(BlueprintType)
struct FHoudiniPresetRampFloatValues : public FHoudiniPresetBase
{
	GENERATED_BODY()

	virtual EHoudiniPresetValueType GetValueType() override { return EHoudiniPresetValueType::String; }

	virtual FString ToString() override { return FString::Format(TEXT("%d float points."), { Points.Num() }); }

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<FHoudiniPresetRampFloatPoint> Points;
};

USTRUCT(BlueprintType)
struct FHoudiniPresetRampColorValues : public FHoudiniPresetBase
{
	GENERATED_BODY()

	virtual EHoudiniPresetValueType GetValueType() override { return EHoudiniPresetValueType::String; }

	virtual FString ToString() override { return FString::Format(TEXT("{0} color points."), { Points.Num() }); }

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<FHoudiniPresetRampColorPoint> Points;
};

USTRUCT(BlueprintType)
struct FHoudiniPresetGeometryInputObject : public FHoudiniPresetBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TSoftObjectPtr<UObject> InputObject;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	FTransform Transform;
};

USTRUCT(BlueprintType)
struct FHoudiniPresetCurveInputObject : public FHoudiniPresetBase
{
	GENERATED_BODY()

	FHoudiniPresetCurveInputObject();

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	FTransform Transform;
	
	// Curve Spline Component

	EHoudiniCurveType GetValidCurveType() const
	{
		if (CurveType == EHoudiniCurveType::Invalid)
		{
			return EHoudiniCurveType::Polygon;
		}
		return CurveType;
	}

	EHoudiniCurveMethod GetValidCurveMethod() const
	{
		if (CurveMethod == EHoudiniCurveMethod::Invalid)
		{
			return EHoudiniCurveMethod::CVs;
		}
		return CurveMethod;
	}

	EHoudiniCurveBreakpointParameterization GetValidCurveBreakpointParameterization() const
	{
		if (CurveBreakpointParameterization == EHoudiniCurveBreakpointParameterization::Invalid)
		{
			return EHoudiniCurveBreakpointParameterization::Uniform;
		}
		return CurveBreakpointParameterization;
	}
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<FTransform> CurvePoints;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bClosed;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bReversed;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	int32 CurveOrder;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bIsHoudiniSplineVisible;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	EHoudiniCurveType CurveType;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	EHoudiniCurveMethod CurveMethod;

	// Only used for new HAPI curve / breakpoints
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	EHoudiniCurveBreakpointParameterization CurveBreakpointParameterization;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bIsOutputCurve;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bCookOnCurveChanged;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bIsLegacyInputCurve;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bIsInputCurve;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bIsEditableOutputCurve;
};


USTRUCT(BlueprintType)
struct FHoudiniPresetInputValue : public FHoudiniPresetBase
{
	GENERATED_BODY()

	virtual EHoudiniPresetValueType GetValueType() override { return EHoudiniPresetValueType::String; }

	virtual FString ToString() override { return FString(); }

	// Export Options
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bKeepWorldTransform;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bPackGeometryBeforeMerging;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bExportInputAsReference;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bExportLODs;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bExportSockets;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bExportColliders;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bExportMaterialParameters;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bMergeSplineMeshComponents;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bPreferNaniteFallbackMesh;
	
	// Input properties

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	EHoudiniInputType InputType;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bIsParameterInput;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	FString ParameterName;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	int32 InputIndex;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<FHoudiniPresetGeometryInputObject> GeometryInputObjects;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<FHoudiniPresetCurveInputObject> CurveInputObjects;
};


struct FHoudiniPresetHelpers
{
	static bool IsSupportedInputType(const EHoudiniInputType InputType);
	
	// Generic version to populate our Preset value structs from Houdini Parameters.
	static void PopulateFromParameter(UHoudiniParameterFloat* Parm, FHoudiniPresetFloatValues& Value);
	static void PopulateFromParameter(UHoudiniParameterInt* Parm, FHoudiniPresetIntValues& Value);
	static void PopulateFromParameter(UHoudiniParameterString* Parm, FHoudiniPresetStringValues& Value);

	// Ingest int parameters
	static bool IngestParameter(UHoudiniParameterInt* Parm, TMap<FString,FHoudiniPresetIntValues>& OutValues, FString& OutValueStr);
	static bool IngestParameter(UHoudiniParameterChoice* Parm, TMap<FString,FHoudiniPresetIntValues>& OutValues, FString& OutValueStr);
	static bool IngestParameter(UHoudiniParameterToggle* Parm, TMap<FString,FHoudiniPresetIntValues>& OutValues, FString& OutValueStr);

	// Ingest float parameters
	static bool IngestParameter(UHoudiniParameterFloat* Parm, TMap<FString,FHoudiniPresetFloatValues>& OutValues, FString& OutValueStr);
	static bool IngestParameter(UHoudiniParameterColor* Parm, TMap<FString,FHoudiniPresetFloatValues>& OutValues, FString& OutValueStr);

	// Ingest string parameters
	static bool IngestParameter(UHoudiniParameterString* Parm, TMap<FString,FHoudiniPresetStringValues>& OutValues, FString& OutValueStr);
	static bool IngestParameter(UHoudiniParameterFile* Parm, TMap<FString,FHoudiniPresetStringValues>& OutValues, FString& OutValueStr);
	static bool IngestParameter(UHoudiniParameterChoice* Parm, TMap<FString,FHoudiniPresetStringValues>& OutValues, FString& OutValueStr);

	// Ingest ramp parameters
	static bool IngestParameter(UHoudiniParameterRampFloat* Parm, TMap<FString,FHoudiniPresetRampFloatValues>& OutValues, FString& OutValueStr);
	static bool IngestParameter(UHoudiniParameterRampColor* Parm, TMap<FString,FHoudiniPresetRampColorValues>& OutValues, FString& OutValueStr);

	// Ingest input parameters
	// static bool IngestParameterInput(UHoudiniParameterOperatorPath* Parm, TArray<FHoudiniPresetInputValue>& OutValues, FString& OutValueStr);
	// static bool IngestAbsoluteInput(int32 InputIndex, UHoudiniInput* Parm, TArray<FHoudiniPresetInputValue>& OutValues, FString& OutValueStr);
	static void IngestGenericInput(UHoudiniInput* Input, bool bIsParameterInput, const FString& ParameterName, TArray<FHoudiniPresetInputValue>& OutValues, FString& OutValueStr);
	static void UpdateGenericInputSettings(FHoudiniPresetInputValue& Value, const UHoudiniInput* Input);
	static void UpdateFromGeometryInput(FHoudiniPresetInputValue& Value, const UHoudiniInput* Input);
	static void UpdateFromCurveInput(FHoudiniPresetInputValue& Value, const UHoudiniInput* Input);


	// Preset Helpers
	
	// Int based parameters
	static void ApplyPresetParameterValues(const FHoudiniPresetIntValues& Values, UHoudiniParameterInt* Param);
	static void ApplyPresetParameterValues(const FHoudiniPresetIntValues& Values, UHoudiniParameterChoice* Param);
	static void ApplyPresetParameterValues(const FHoudiniPresetIntValues& Values, UHoudiniParameterToggle* Param);

	// Float based parameters
	static void ApplyPresetParameterValues(const FHoudiniPresetFloatValues& Values, UHoudiniParameterFloat* Param);
	static void ApplyPresetParameterValues(const FHoudiniPresetFloatValues& Values, UHoudiniParameterColor* Param);

	// String based parameters
	static void ApplyPresetParameterValues(const FHoudiniPresetStringValues& Values, UHoudiniParameterString* Param);
	static void ApplyPresetParameterValues(const FHoudiniPresetStringValues& Values, UHoudiniParameterFile* Param);
	static void ApplyPresetParameterValues(const FHoudiniPresetStringValues& Values, UHoudiniParameterChoice* Param);

	// Ramp based parameters
	static void ApplyPresetParameterValues(const FHoudiniPresetRampFloatValues& Values, UHoudiniParameterRampFloat* Param);
	static void ApplyPresetParameterValues(const FHoudiniPresetRampColorValues& Values, UHoudiniParameterRampColor* Param);

	// Input parameters
	static void ApplyPresetParameterValues(const FHoudiniPresetInputValue& PresetInput, UHoudiniInput* Input);

protected:
	static void ApplyPresetGeometryInput(const FHoudiniPresetInputValue& PresetInput, UHoudiniInput* Input);
	static void ApplyPresetCurveInput(const FHoudiniPresetInputValue& PresetInput, UHoudiniInput* Input);
};


UCLASS(BlueprintType, hidecategories=(Object))
class HOUDINIENGINEEDITOR_API UHoudiniPreset : public UObject
{
	GENERATED_BODY()
public:

	UHoudiniPreset();

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif

	// The label for this preset
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	FString Name;

	// A description for this preset
	UPROPERTY(EditAnywhere, Category="Houdini Preset", meta=(MultiLine="true"))
	FString Description;

	// The HoudiniAsset linked to this preset.
	// Should this be a soft object pointer instead?
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	UHoudiniAsset* SourceHoudiniAsset;
	
	// Whether the revert all parameters on the HDA to their default values before applying this preset
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bRevertHDAParameters;

	// Whether to treat this preset as hidden (hide from preset menus and will be not be visible in HoudiniTools Panel). 
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bHidePreset;

	// Whether this preset can be applied to any HDA, or only the SourceHoudiniAsset.
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bApplyOnlyToSource;

	// Whether this preset be instantiated (using the SourceHoudiniAsset).
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bCanInstantiate;

	// Cook and Bake Folders
	// We add toggles specifically for temp/bake folders since we might want to
	// control them separately respective options groups.
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bApplyTemporaryCookFolder;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	FString TemporaryCookFolder;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bApplyBakeFolder;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	FString BakeFolder;
	
	// Bake Options

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bApplyBakeOptions;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	EHoudiniEngineBakeOption HoudiniEngineBakeOption;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bRemoveOutputAfterBake;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bRecenterBakedActors;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bAutoBake;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bReplacePreviousBake;
	
	// Asset Options

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bApplyAssetOptions;

	// Asset Options - Cook Triggers

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bCookOnParameterChange;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bCookOnTransformChange;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bCookOnAssetInputCook;

	// Asset Options - Outputs

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bDoNotGenerateOutputs;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bUseOutputNodes;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bOutputTemplateGeos;

	// Asset Options - Misc

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bUploadTransformsToHoudiniEngine;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bLandscapeUseTempLayers;

	
	// Parameters

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TMap<FString, FHoudiniPresetFloatValues> FloatParameters;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TMap<FString, FHoudiniPresetIntValues> IntParameters;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TMap<FString, FHoudiniPresetStringValues> StringParameters;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TMap<FString, FHoudiniPresetRampFloatValues> RampFloatParameters;
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TMap<FString, FHoudiniPresetRampColorValues> RampColorParameters;

	// Inputs
	
	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	TArray<FHoudiniPresetInputValue> InputParameters;

	// Static Mesh Generation Settings

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bApplyStaticMeshGenSettings;

	
	UPROPERTY(Category = "HoudiniMeshGeneration", EditAnywhere)
	bool bUseDeprecatedRawMeshSupport;

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	FHoudiniStaticMeshGenerationProperties StaticMeshGenerationProperties;
	
	UPROPERTY(Category = "HoudiniMeshGeneration", EditAnywhere)
	FMeshBuildSettings StaticMeshBuildSettings;
	
	// Proxy Mesh Gen Settings

	UPROPERTY(EditAnywhere, Category="Houdini Preset")
	bool bApplyProxyMeshGenSettings;

	UPROPERTY(Category = "HoudiniProxyMeshGeneration", EditAnywhere)
	bool bOverrideGlobalProxyStaticMeshSettings;

	UPROPERTY(Category = "HoudiniProxyMeshGeneration", EditAnywhere, meta = (DisplayName="Enable Proxy Static Mesh"))
	bool bEnableProxyStaticMeshOverride;
	
	UPROPERTY(Category = "HoudiniProxyMeshGeneration", EditAnywhere, meta = (DisplayName="Refine Proxy Static Meshes After a Timeout"))
	bool bEnableProxyStaticMeshRefinementByTimerOverride;
	
	UPROPERTY(Category = "HoudiniProxyMeshGeneration", EditAnywhere, meta = (DisplayName="Proxy Mesh Auto Refine Timeout Seconds"))
	float ProxyMeshAutoRefineTimeoutSecondsOverride;
	
	UPROPERTY(Category = "HoudiniProxyMeshGeneration", EditAnywhere, meta = (DisplayName="Refine Proxy Static Meshes When Saving a Map"))
	bool bEnableProxyStaticMeshRefinementOnPreSaveWorldOverride;
	
	UPROPERTY(Category = "HoudiniProxyMeshGeneration", EditAnywhere, meta = (DisplayName="Refine Proxy Static Meshes On PIE"))
	bool bEnableProxyStaticMeshRefinementOnPreBeginPIEOverride;

	// Raw image data of the icon to be displayed
	UPROPERTY()
	FHImageData IconImageData;
};
