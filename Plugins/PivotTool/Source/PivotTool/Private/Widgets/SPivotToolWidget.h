// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "PivotToolUtil.h"
#include "SPivotPreset.h"
#include "SPivotAlign.h"

class SPivotToolWidget : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SPivotToolWidget) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

	//~ Begin SWidget Interface
	virtual bool SupportsKeyboardFocus() const override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~ End SWidget Interface

	// Save/Load UIStting
	void SaveUISetting();
	void LoadUISetting();

private:

	FText GetActorPivotHeaderText() const;
	FText GetMeshPivotHeaderText() const;
	FText GetMatchActorHeaderText() const;

	FReply OnApplyOffsetClicked();
	bool IsApplyOffsetEnabled() const;

	FReply OnSnapToVertexClicked();
	bool IsSnapToVertexEnabled() const;

	FReply OnSetClicked();
	FReply OnResetClicked();
	FReply OnCopyClicked();
	FReply OnPasteClicked();
	bool IsPasteEnabled() const;

	bool IsBakeEnabled() const;
	FReply OnBakeClicked();
	FReply OnDuplicateAndBakeClicked();

	bool IsBakeByActorEnabled() const;
	FReply OnBakeByActorLocClicked();
	FReply OnBakeByActorRotClicked();

	FText GetAlignActorsCheckboxText() const;
	FText GetCopyInWorldSpaceCheckboxText() const;
	FText GetFreezeRotationCheckboxText() const;
	FText GetFreezeScaleCheckboxText() const;
	FText GetKeepActorsInPlaceCheckboxText() const;
	FText GetDisplayMatchActorCheckboxText() const;

	AActor* GetLastSelectedActor() const;

	FReply OnPivotPresetSelected(EPivotPreset InPreset);
	void OnPivotPresetHovered(EPivotPreset InPreset);
	void OnPivotPresetUnhovered(EPivotPreset InPreset);

	TOptional<float> GetPivotOffsetInputX() const { return PivotOffsetInput.X; }
	TOptional<float> GetPivotOffsetInputY() const { return PivotOffsetInput.Y; }
	TOptional<float> GetPivotOffsetInputZ() const { return PivotOffsetInput.Z; }
	void OnSetPivotOffsetInput(float InValue, int32 InAxis);

	void TickPreviewPivots();
	void TickUIWrapping();

	void UpdateOptions(FPivotToolOptions& Options);

	TSharedRef<SWidget> CreateChangeLogWidget();
	TSharedPtr<class IDocumentationPage> AboutPage;
	bool bShowChangelog = false;

private:

	bool bDisplayAlignActors = false;
	bool bDisplayMatchActors = false;

	bool bFreezeRotation = false;
	bool bFreezeScale = false;
	bool bKeepActorsInPlace = false;

	bool bBakeByActor = false;
	bool bBakeByActorDuplicate = false;
	bool bBakeByActorLoc = false;
	bool bBakeByActorRot = false;

	bool bCopyPivotInWorldSpace = false;

	bool bIsCtrlDown;
	bool bIsShiftDown;
	bool bIsAltDown;

	float WidthLastFrame = 0.f;

	FVector PivotOffsetInput;

	TSharedPtr<SPivotPreset> PivotPreset;
	TSharedPtr<SWrapBox> ApplyOffsetWrapBox;
	TSharedPtr<SWrapBox> MainToolboxWrapBox;
	TSharedPtr<SPivotAlign> AlignActorsWidget;

	TArray<FTransform> PreviewPivots;
};
