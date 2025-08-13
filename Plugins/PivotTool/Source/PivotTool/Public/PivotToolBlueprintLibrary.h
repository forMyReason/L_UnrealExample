// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "PivotTool.h"
#include "PivotToolTypes.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "PivotToolBlueprintLibrary.generated.h"

class UStaticMesh;

UCLASS(meta = (ScriptName = "PivotToolLibrary"))
class PIVOTTOOL_API UPivotToolBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Bake Pivot")
	static void BakeMeshPivot(UStaticMesh* StaticMesh, EPivotPreset PivotPreset = EPivotPreset::BoundingBoxCenterBottom, FRotator Rotaion = FRotator::ZeroRotator, FVector Scale = FVector(1,1,1));

	UFUNCTION(BlueprintCallable, Category = "Bake Pivot")
	static void BakeMeshPivotRelative(UStaticMesh* StaticMesh, FVector PivotLocation = FVector::ZeroVector, FRotator PivotRotaion = FRotator::ZeroRotator, FVector Scale = FVector(1,1,1),
		bool bBakeLocation = true, bool bBakeRotation = false, bool bBakeScale = false);
};
