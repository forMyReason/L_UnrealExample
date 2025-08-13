// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnrealWidget.h"

#include "PivotToolTypes.h"

struct FKAggregateGeom;
struct FRawMesh;
struct FMeshDescription;
class AActor;

struct FPivotToolOptions
{
	bool bAutoSave = false;
	bool bGroupMode = false;
	bool bLastSelectedMode = false;
	bool bVertexMode = false;
	bool bChildrenMode = false;

	bool bFreezeRotation = false;
	bool bFreezeScale = false;
	bool bKeepActorsInPlace = false;

	bool bBakeByActor = false;
	bool bBakeByActorLoc = false;
	FTransform BakeByActorTransform;
};

struct FPivotToolUtil
{
	static void CopyActorPivotOffset(bool bWorldSpace);
	static void PasteActorPivotOffset(bool bWorldSpace);
	
	static void OffsetSelectedActorsPivotOffset(const FVector& NewPivotOffset, bool bWorldSpace = false);
	static void SnapSelectedActorsPivotToVertex();

	static void UpdateSelectedActorsPivotOffset(const FVector& NewPivotOffset, bool bWorldSpace = false);
	static void UpdateSelectedActorsPivotOffset(EPivotPreset InPivotPrest, const FPivotToolOptions& InOptions);
	static void GetPreviewPivotsOfSelectedActors(EPivotPreset InPivotPrest, const FPivotToolOptions& InOptions, TArray<FTransform>& OutWorldPivots);

	static bool BakeStaticMeshPivotOffset(class UStaticMesh* StaticMesh, const FVector& PivotOffset, const FQuat& Rotation, const FVector& Scale, bool bBakeRotation, bool bBakeScale, bool bSilentMode = false);
	static bool BakeStaticMeshActorsPivotOffsetToRawMesh(class AStaticMeshActor* InStaticMeshActor, const FPivotToolOptions& InOptions, bool bDuplicate = false, bool bSilentMode = false);

	static void NotifyMessage(const FText& Message, float InDuration);

	static bool HasPivotOffsetCopied() { return bPivotOffsetCopied; }

	static bool UpdateActorPivotOffset(AActor* InActor, const FVector& NewPivotOffset, bool bWorldSpace = false);
	
	static void AlignSelectionPivots(ECoordSystem InCoordSystem, EAxis::Type Axis);// , bool bNegative = false, bool bEnableUndo = true, bool bNotify = true);
	static void AlignToLastSelectedActorsWithPivotOffset(ECoordSystem InCoordSystem, EAxis::Type Axis, bool bMirrored = false);
	static void AlignSelectedActorsWithPivotOffset(ECoordSystem InCoordSystem, EAxis::Type Axis, bool bMirrored = false);
	static void AlignSelectionRotation(EAxis::Type Axis);

	static FVector GetBoundingBoxPoint(const FBox& InBoundingBox, EPivotPreset InPreset);
private:

	static void TransformRawMeshVertexData(const FTransform3f& InTransform, FRawMesh& OutRawMesh, const FVector3f& InBuildScale3D, bool bFreezeBuildScale = false);
	static void TransformMeshVertexData(const FTransform& InTransform, FMeshDescription* OutMesh, const FVector& InBuildScale3D, bool bFreezeBuildScale = false);
	static void TransformPhysicsGemotry(const FTransform& InTransform, FKAggregateGeom& AggGeom);

	static FVector GetActorPivot(AActor* InActor, EPivotPreset InPreset, bool bVertexMode, bool bChildrenMode, bool bLocalSpace = true);
	static bool GetActorVertexPostion(AActor* InActor, EPivotPreset InPreset, FVector& OutPosition);
	static FVector GetActorBoundingBoxPoint(AActor* InActor, EPivotPreset InPreset, bool bChildrenMode, bool bLocalSpace = true);
	static FBoxSphereBounds GetSelectionBounds(bool bChildrenMode = false);

	static FBox GetAllComponentsBoundingBox(AActor* InActor, bool bRecursive);

	static FVector CopiedPivotOffset;
	static bool bPivotOffsetCopied;
};
