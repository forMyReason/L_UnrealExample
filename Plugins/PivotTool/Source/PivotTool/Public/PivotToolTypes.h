// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PivotToolTypes.generated.h"

UENUM(BlueprintType)
enum class EPivotPreset : uint8
{
	BoundingBoxCenter UMETA(DisplayName = "Center"),

	BoundingBoxCenterTop UMETA(DisplayName = "CenterTop"),
	BoundingBoxCenterBottom UMETA(DisplayName = "CenterBottom"),
	BoundingBoxCenterLeft UMETA(DisplayName = "CenterLeft"),
	BoundingBoxCenterRight UMETA(DisplayName = "CenterRight"),
	BoundingBoxCenterFront UMETA(DisplayName = "CenterFront"),
	BoundingBoxCenterBack UMETA(DisplayName = "CenterBack"),

	/*******************
	   2 _________ 3
	    /        /
	   /        /
	1 /________/ 4

	******************/
	BoundingBoxCornerTop1 UMETA(DisplayName = "TopLeftFront"),
	BoundingBoxCornerTop2 UMETA(DisplayName = "TopLeftBack"),
	BoundingBoxCornerTop3 UMETA(DisplayName = "TopRightFront"),
	BoundingBoxCornerTop4 UMETA(DisplayName = "TopRightBack"),
	BoundingBoxCornerBottom1 UMETA(DisplayName = "BottomLeftFront"),
	BoundingBoxCornerBottom2 UMETA(DisplayName = "BottomLeftBack"),
	BoundingBoxCornerBottom3 UMETA(DisplayName = "BottomRightFront"),
	BoundingBoxCornerBottom4 UMETA(DisplayName = "BottomRightBack"),

	/*******************
	    ___ 2____
	   /        /
	  1        3
	 /___ 4 __/ 

	******************/
	BoundingBoxEdgeTop1 UMETA(DisplayName = "TopEdgeLeft"),
	BoundingBoxEdgeTop2 UMETA(DisplayName = "TopEdgeBack"),
	BoundingBoxEdgeTop3 UMETA(DisplayName = "TopEdgeRight"),
	BoundingBoxEdgeTop4 UMETA(DisplayName = "TopEdgeFront"),
	BoundingBoxEdgeMid1 UMETA(DisplayName = "MidEdgeLeft"),
	BoundingBoxEdgeMid2 UMETA(DisplayName = "MidEdgeBack"),
	BoundingBoxEdgeMid3 UMETA(DisplayName = "MidEdgeRight"),
	BoundingBoxEdgeMid4 UMETA(DisplayName = "MidEdgeFront"),
	BoundingBoxEdgeBottom1 UMETA(DisplayName = "BottomEdgeLeft"),
	BoundingBoxEdgeBottom2 UMETA(DisplayName = "BottomEdgeBack"),
	BoundingBoxEdgeBottom3 UMETA(DisplayName = "BottomEdgeRight"),
	BoundingBoxEdgeBottom4 UMETA(DisplayName = "BottomEdgeFront"),

	MAX,
};
