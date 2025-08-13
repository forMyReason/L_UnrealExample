// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "PivotToolStyle.h"

#include "Framework/Commands/Commands.h"

struct FPivotToolCommandTooltip
{
	FText BasicTooltip;
	FString Link;
	FString ExcerptName;

	FPivotToolCommandTooltip(const FString& InBasicTooltip, const FString& InLink, const FString InExcerptName)
		: BasicTooltip(FText::FromString(InBasicTooltip))
		, Link(InLink)
		, ExcerptName(InExcerptName)
	{}
};

class FPivotToolCommands : public TCommands<FPivotToolCommands>
{
public:

	FPivotToolCommands()
		: TCommands<FPivotToolCommands>(TEXT("PivotTool"), NSLOCTEXT("Contexts", "PivotTool", "PivotTool Plugin"), NAME_None, FPivotToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedRef<class SToolTip> CreateToolTip(const TSharedPtr<FUICommandInfo>& InUICommandInfo, bool bAlwaysShowFullTooltip = false) const;
	bool HasFullTooltip(const TSharedPtr<FUICommandInfo>& InCommand) const;
private:
	void AddFullTooltip(const TSharedPtr<FUICommandInfo>& InCommand, const FString& InLink);
	FString GetBasicToolTip(const TSharedPtr<class FUICommandInfo>& CommandInfo) const;

	TMap<FUICommandInfo*, FPivotToolCommandTooltip> CommandTooltips;

public:
	TSharedPtr< FUICommandInfo > OpenPivotTool;

	// Actor Pivot
	TSharedPtr<FUICommandInfo> ApplyPivotOffset;
	TSharedPtr<FUICommandInfo> SnapPivotToVert;

	TSharedPtr<FUICommandInfo> SetActorPivot;
	TSharedPtr<FUICommandInfo> ResetActorPivot;
	TSharedPtr<FUICommandInfo> CopyActorPivot;
	TSharedPtr<FUICommandInfo> PasteActorPivot;
	TSharedPtr<FUICommandInfo> ToggleCopyActorPivotInWorldSpace;

	// Mesh Pivot
	TSharedPtr<FUICommandInfo> BakeMeshPivot;
	TSharedPtr<FUICommandInfo> DuplicateBakeMeshPivot;
	TSharedPtr<FUICommandInfo> ToggleBakeFreezeRotation;
	TSharedPtr<FUICommandInfo> ToggleBakeFreezeScale;
	TSharedPtr<FUICommandInfo> ToggleBakeInPlace;

	TSharedPtr<FUICommandInfo> ToggleBakeByActor;
	TSharedPtr<FUICommandInfo> BakeByActorLoc;
	TSharedPtr<FUICommandInfo> BakeByActorRot;

	// Align Actors
	TSharedPtr<FUICommandInfo> AlignActorsAll;
	TSharedPtr<FUICommandInfo> AlignActorsX;
	TSharedPtr<FUICommandInfo> AlignActorsNegX;
	TSharedPtr<FUICommandInfo> AlignActorsY;
	TSharedPtr<FUICommandInfo> AlignActorsNegY;
	TSharedPtr<FUICommandInfo> AlignActorsZ;
	TSharedPtr<FUICommandInfo> AlignActorsNegZ;
};