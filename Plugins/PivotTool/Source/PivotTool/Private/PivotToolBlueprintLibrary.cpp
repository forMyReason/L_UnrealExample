// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "PivotToolBlueprintLibrary.h"
#include "PivotToolUtil.h"

#include "RawMesh.h"

void UPivotToolBlueprintLibrary::BakeMeshPivot(UStaticMesh* StaticMesh, EPivotPreset PivotPreset, FRotator Rotaion, FVector Scale)
{
	if (StaticMesh)
	{
		FBox BoundingBox = StaticMesh->GetBoundingBox();
		PIVOTTOOL_LOG(Display, TEXT("[BakeMeshPivot] BB Center: %s, Extent: %s"), *BoundingBox.GetCenter().ToString(), *BoundingBox.GetExtent().ToString());

		FVector PivotOffset = FPivotToolUtil::GetBoundingBoxPoint(BoundingBox, PivotPreset);// -BoundingBox.GetCenter();
		PIVOTTOOL_LOG(Display, TEXT("     PivotOffset: %s"), *PivotOffset.ToString());

		FPivotToolUtil::BakeStaticMeshPivotOffset(StaticMesh, PivotOffset, Rotaion.Quaternion(), /*Scale*/Scale, /*bBakeRotation =*/true, /*bBakeScale =*/true, /*bSilentMode =*/false);
	}
}

void UPivotToolBlueprintLibrary::BakeMeshPivotRelative(UStaticMesh* StaticMesh, FVector Location /*= FVector::ZeroVector*/, FRotator Rotaion /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1,1,1)*/, bool bBakeLocation /*= true*/, bool bBakeRotation /*= false*/, bool bBakeScale /*= false*/)
{
	FPivotToolUtil::BakeStaticMeshPivotOffset(StaticMesh, Location, Rotaion.Quaternion(), /*Scale*/Scale, /*bBakeRotation =*/true, /*bBakeScale =*/true, /*bSilentMode =*/false);
}
