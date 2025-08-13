// Copyright 2017-2021 marynate. All Rights Reserved.

#include "SPivotToolWidget.h"
#include "PivotToolUtil.h"
#include "PivotToolSettings.h"
#include "PivotToolStyle.h"
#include "PivotTool.h"
#include "PivotToolCommands.h"

#include "LevelEditorActions.h"
#include "EditorModeManager.h"
#include "StaticMeshResources.h"
#include "RawMesh.h"
#include "EditorSupportDelegates.h"
#include "Interfaces/IPluginManager.h"
#include "Engine/Selection.h"
#include "Engine/StaticMeshActor.h"
#include "Widgets/Input/SVectorInputBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "DrawDebugHelpers.h"
#include "Misc/ConfigCacheIni.h"
#include "DetailLayoutBuilder.h"

#define LOCTEXT_NAMESPACE "SPivotToolWidget"

#define HORIZONTAL_SPACER + SHorizontalBox::Slot().FillWidth(1.f) [ SNew(SSpacer) ]
#define VERTICAL_SPACER + SVerticalBox::Slot().FillHeight(1.f) [ SNew(SSpacer) ]

namespace PivotToolWidgetConstants
{
	const float HideActorPivotTextWidth = 270.f;
	const float HideMeshPivotTextWidth = 270.f;
}

void SPivotToolWidget::Construct(const FArguments& InArgs)
{
	LoadUISetting();

	bDisplayMatchActors = false;

	if (!bDisplayMatchActors)
		bBakeByActor = false;

	WidthLastFrame = 0.f;

	PivotOffsetInput = FVector::ZeroVector;

	bIsCtrlDown = false;
	bIsShiftDown = false;
	bIsAltDown = false;

	PreviewPivots.Empty();

	const FText VersionName = FText::FromString(IPluginManager::Get().FindPlugin(TEXT("PivotTool"))->GetDescriptor().VersionName);
	const FText CreatedBy = FText::FromString(TEXT("by ") + IPluginManager::Get().FindPlugin(TEXT("PivotTool"))->GetDescriptor().CreatedBy);
	const FText FriendlyName = FText::FromString(IPluginManager::Get().FindPlugin(TEXT("PivotTool"))->GetDescriptor().FriendlyName);

	const FText PivotToolVersion = FText::Format(LOCTEXT("PivotToolVersion", "{0} {1}"), FriendlyName, VersionName);

	const float ButtonContentPadding = 4.f;
	const float ButtonTextMinDesiredWidth = 20.f;
	const float VerticalPadding = 3.f;

	TSharedPtr<SBorder> MainContent;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SAssignNew(MainContent, SBorder)
			.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f))
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(0.f)
		]
		// Change log
		+ SOverlay::Slot()[CreateChangeLogWidget()]
	];

	MainContent->SetContent(

		SNew(SScrollBox)
		.Orientation(Orient_Vertical)
		.ScrollBarAlwaysVisible(false)
		+ SScrollBox::Slot()
		[

		SNew(SVerticalBox)

		// Pivot Preset >>
#if PIVOTTOOL_FOLD
		+ SVerticalBox::Slot().Padding(0, VerticalPadding, 0, 0).AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			[
				SNew(SBorder)
				//.BorderBackgroundColor(FLinearColor(.1f,.1f,.1f))
				.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f))
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(2.0f)
				[
					SAssignNew(PivotPreset, SPivotPreset)
					.OnSelected(this, &SPivotToolWidget::OnPivotPresetSelected)
					.OnHovered(this, &SPivotToolWidget::OnPivotPresetHovered)
					.OnUnhovered(this, &SPivotToolWidget::OnPivotPresetUnhovered)
				]
			]
		]
#endif	// Pivot Preset <<

		// Pivot UI >>
#if PIVOTTOOL_FOLD
		+ SVerticalBox::Slot().Padding(0, VerticalPadding, 0, 0).FillHeight(1.f)
		[
			SNew(SVerticalBox)

			// Pivot Offset >>
#if PIVOTTOOL_FOLD
			+ SVerticalBox::Slot().Padding(0, VerticalPadding).AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				.Padding(2, 0)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SAssignNew(ApplyOffsetWrapBox, SWrapBox)
					.InnerSlotPadding(FVector2D(4.f, 2.f))

					// Pivot Offset Input
					+ SWrapBox::Slot().HAlign(HAlign_Center)
					[
						SNew(SHorizontalBox)

						// Apply Offset Input
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(2, 0)
						[
							SNew(SVectorInputBox)
							.Font(IDetailLayoutBuilder::GetDetailFont())
							.Visibility(MakeAttributeLambda([=, this] {return WidthLastFrame >= 180.f ? EVisibility::Visible : EVisibility::Collapsed; }))
							.bColorAxisLabels(true)
							.AllowSpin(true)
							.X(this, &SPivotToolWidget::GetPivotOffsetInputX)
							.Y(this, &SPivotToolWidget::GetPivotOffsetInputY)
							.Z(this, &SPivotToolWidget::GetPivotOffsetInputZ)
							.OnXChanged(this, &SPivotToolWidget::OnSetPivotOffsetInput, 0)
							.OnYChanged(this, &SPivotToolWidget::OnSetPivotOffsetInput, 1)
							.OnZChanged(this, &SPivotToolWidget::OnSetPivotOffsetInput, 2)
						]

						// Apply Offset Button
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(2, 0)
						[
							SNew(SButton)
							.Visibility(MakeAttributeLambda([=, this] {return WidthLastFrame >= 180.f ? EVisibility::Visible : EVisibility::Collapsed; }))
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ContentPadding(ButtonContentPadding)
							.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.DarkGray"))
							.OnClicked(this, &SPivotToolWidget::OnApplyOffsetClicked)
							.IsEnabled(MakeAttributeLambda([=, this] { return !bBakeByActor; }))
							.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().ApplyPivotOffset))
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
								.Text(LOCTEXT("Offset", "Offset"))
								.MinDesiredWidth(ButtonTextMinDesiredWidth)
								.Justification(ETextJustify::Center)
							]
						]
					]

					+ SWrapBox::Slot().HAlign(HAlign_Center)
					[
#if PIVOTTOOL_FOLD
						SNew(SHorizontalBox)

						// Snap to Vertex
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(2, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ContentPadding(ButtonContentPadding)
							.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.DarkGray"))
							.OnClicked(this, &SPivotToolWidget::OnSnapToVertexClicked)
							.IsEnabled(this, &SPivotToolWidget::IsSnapToVertexEnabled)
							.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().SnapPivotToVert))
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
								.Text(LOCTEXT("SnapToVertex", "Snap to Vert"))
								.MinDesiredWidth(ButtonTextMinDesiredWidth)
								.Justification(ETextJustify::Center)
							]
						]
#endif
					]
				]
			]
#endif		// Pivot Offset <<

			// Actor Pivots
#if PIVOTTOOL_FOLD
			+ SVerticalBox::Slot().Padding(0, VerticalPadding, 0, 0).AutoHeight()
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.4f, 0.4f, 0.4f))
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.HAlign(HAlign_Center)
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.AutoWidth().Padding(4, 0)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ActorPivot:", "Actor Pivot:"))
						.Text(this, &SPivotToolWidget::GetActorPivotHeaderText)
						.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText.DarkerGray")
						.Justification(ETextJustify::Center)
					]

					// Set
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ContentPadding(ButtonContentPadding)
						.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
						.OnClicked(this, &SPivotToolWidget::OnSetClicked)
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().SetActorPivot))
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
							.Text(LOCTEXT("Set", "Set"))
							.MinDesiredWidth(ButtonTextMinDesiredWidth)
							.Justification(ETextJustify::Center)
						]
					]
#endif
					// Reset
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ContentPadding(ButtonContentPadding)
						.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
						.OnClicked(this, &SPivotToolWidget::OnResetClicked)
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().ResetActorPivot))
						[
							SNew(STextBlock)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
							.Text(LOCTEXT("Reset", "Reset"))
							.MinDesiredWidth(ButtonTextMinDesiredWidth)
							.Justification(ETextJustify::Center)
						]
					]
#endif

					+ SHorizontalBox::Slot().AutoWidth().Padding(2, 0)[SNew(SSpacer)]

					// Copy
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ContentPadding(ButtonContentPadding)
						.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
						.OnClicked(this, &SPivotToolWidget::OnCopyClicked)
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().CopyActorPivot))
						[
							SNew(STextBlock)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
							.Text(LOCTEXT("Copy", "Copy"))
							.MinDesiredWidth(ButtonTextMinDesiredWidth)
							.Justification(ETextJustify::Center)
						]
					]
#endif
					// Paste
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0, 0, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ContentPadding(ButtonContentPadding)
						.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
						.OnClicked(this, &SPivotToolWidget::OnPasteClicked)
						.IsEnabled(this, &SPivotToolWidget::IsPasteEnabled)
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().PasteActorPivot))
						[
							SNew(STextBlock)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
							.Text(LOCTEXT("Paste", "Paste"))
							.MinDesiredWidth(ButtonTextMinDesiredWidth)
							.Justification(ETextJustify::Center)
						]
					]
#endif
					// Copy Past in World Space?
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 2, 0, 2)
					.VAlign(VAlign_Center)
					[
						SNew(SCheckBox)
						.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
						{
								bCopyPivotInWorldSpace = State == ECheckBoxState::Checked;
								SaveUISetting();
						})
						.IsChecked(MakeAttributeLambda([=, this] {return bCopyPivotInWorldSpace ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }))
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().ToggleCopyActorPivotInWorldSpace))
						.Content()
						[
							SNew(STextBlock)
							.Text(this, &SPivotToolWidget::GetCopyInWorldSpaceCheckboxText)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText")
							.Justification(ETextJustify::Center)
						]
					]

					+ SHorizontalBox::Slot().AutoWidth().Padding(2, 0)[SNew(SSpacer)]
#endif

					+ SHorizontalBox::Slot().AutoWidth().Padding(2, 0)[SNew(SSpacer)]

					// Display Align Actors
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 2, 0, 2)
					[
						SNew(SCheckBox)
						.Style(&FPivotToolStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PivotTool.ToggleButtonCheckbox"))
						.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
						{
							bDisplayAlignActors = State == ECheckBoxState::Checked;
							SaveUISetting();
						})
						.IsChecked(MakeAttributeLambda([=, this] {return bDisplayAlignActors ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }))
						.ToolTipText(LOCTEXT("AutoSaveToolTip", "Toggles whether auto save pivot offset"))
						.Padding(2)
						.Content()
						[
							SNew(SBox).VAlign(VAlign_Center).HAlign(HAlign_Center).Padding(FMargin(0, 0))
							[
								SNew(STextBlock)
								.Text(this, &SPivotToolWidget::GetAlignActorsCheckboxText)
								.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText")
							]
						]
					]
#endif

				]
			] // End of Main Tool bar
#endif

			  // Align Actors
#if PIVOTTOOL_FOLD
			+ SVerticalBox::Slot().Padding(0, VerticalPadding, 0, 0).AutoHeight()
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f))
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.HAlign(HAlign_Center)
				.Visibility(MakeAttributeLambda([=, this] {return bDisplayAlignActors ? EVisibility::Visible : EVisibility::Collapsed; }))
				[
					SAssignNew(AlignActorsWidget, SPivotAlign)
				]
			]
#endif

			// Mesh Pivots
#if PIVOTTOOL_FOLD
			+ SVerticalBox::Slot().Padding(0, VerticalPadding, 0, 0).AutoHeight()
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.4f, 0.4f, 0.4f))
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.HAlign(HAlign_Center)
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.AutoWidth().Padding(4, 0)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(this, &SPivotToolWidget::GetMeshPivotHeaderText)
						.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText.DarkerGray")
						.Justification(ETextJustify::Center)
					]

					// Bake
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ContentPadding(ButtonContentPadding)
						.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
						.OnClicked(this, &SPivotToolWidget::OnBakeClicked)
						.IsEnabled(this, &SPivotToolWidget::IsBakeEnabled)
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().BakeMeshPivot))
						[
							SNew(STextBlock)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
							.Text(LOCTEXT("Bake", "Bake"))
							.MinDesiredWidth(ButtonTextMinDesiredWidth)
							.Justification(ETextJustify::Center)
						]
					]
#endif
				// Duplicate and Bake
#if PIVOTTOOL_FOLD
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 0, 0, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ContentPadding(ButtonContentPadding)
				.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
				.OnClicked(this, &SPivotToolWidget::OnDuplicateAndBakeClicked)
				.IsEnabled(this, &SPivotToolWidget::IsBakeEnabled)
				.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().BakeMeshPivot))
				[
					SNew(STextBlock)
					.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
				.Text(LOCTEXT("DuplicateAndBake", "D.Bake"))
				.MinDesiredWidth(ButtonTextMinDesiredWidth)
				.Justification(ETextJustify::Center)
				]
				]
#endif
				// Freeze Rotation
#if PIVOTTOOL_FOLD
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 2, 2, 2)
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
					{
						bFreezeRotation = State == ECheckBoxState::Checked;
						SaveUISetting();
					})
					.IsChecked(MakeAttributeLambda([=, this] {return bFreezeRotation ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }))
					.IsEnabled(MakeAttributeLambda([=, this] { return !bBakeByActor; }))
					.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().ToggleBakeFreezeRotation))
					.Content()
					[
						SNew(STextBlock)
						.Text(this, &SPivotToolWidget::GetFreezeRotationCheckboxText)
					.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText")
					.Justification(ETextJustify::Center)
					]
				]
#endif

				// Freeze Scale
#if PIVOTTOOL_FOLD
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 2, 2, 2)
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
					{
						bFreezeScale = State == ECheckBoxState::Checked;
						SaveUISetting();
					})
					.IsChecked(MakeAttributeLambda([=, this] {return bFreezeScale ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }))
					.IsEnabled(MakeAttributeLambda([=, this] { return !bBakeByActor; }))
					.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().ToggleBakeFreezeRotation))
					.Content()
					[
						SNew(STextBlock)
						.Text(this, &SPivotToolWidget::GetFreezeScaleCheckboxText)
						.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText")
						.Justification(ETextJustify::Center)
					]
				]
#endif

				// Maintain actors location after bake
#if PIVOTTOOL_FOLD
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 2, 0, 2)
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
					{
						bKeepActorsInPlace = State == ECheckBoxState::Checked;
						SaveUISetting();
					})
					.IsChecked(MakeAttributeLambda([=, this] {return bKeepActorsInPlace ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }))
					.IsEnabled(MakeAttributeLambda([=, this] {return !bBakeByActor; }))
					.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().ToggleBakeInPlace))
					.Content()
					[
						SNew(STextBlock)
						.Text(this, &SPivotToolWidget::GetKeepActorsInPlaceCheckboxText)
						.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText")
						.Justification(ETextJustify::Center)
					]
				]
#endif

				+ SHorizontalBox::Slot().AutoWidth().Padding(2, 0)[SNew(SSpacer)]

				// Display Match Actors
#if PIVOTTOOL_FOLD
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2, 2, 0, 2)
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
					.Style(&FPivotToolStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PivotTool.ToggleButtonCheckbox"))
					.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
						{
							bDisplayMatchActors = State == ECheckBoxState::Checked;
							if (!bDisplayMatchActors)
								bBakeByActor = false;
							SaveUISetting();
						})
					.IsChecked(MakeAttributeLambda([=, this] {return bDisplayMatchActors ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }))
					//.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().ToggleBakeByActor))
					.Padding(2)
					.Content()
					[
						SNew(SBox).VAlign(VAlign_Center).HAlign(HAlign_Center).Padding(FMargin(0, 0))
						[
							SNew(STextBlock)
							.Text(this, &SPivotToolWidget::GetDisplayMatchActorCheckboxText)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText")
						]
					]
				]
#endif


				]
			] // End of Main Tool bar
#endif
			
			// Bake by Actor
#if PIVOTTOOL_FOLD
			+ SVerticalBox::Slot().Padding(0, VerticalPadding, 0, 0).AutoHeight()
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f))
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.HAlign(HAlign_Center)
				.Visibility(MakeAttributeLambda([=, this] {return bDisplayMatchActors ? EVisibility::Visible : EVisibility::Collapsed; }))
				[
					SNew(SHorizontalBox)
#if 0
					+ SHorizontalBox::Slot()
					.AutoWidth().Padding(4, 0)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(this, &SPivotToolWidget::GetMatchActorHeaderText)
						.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText.DarkerGray")
						.Justification(ETextJustify::Center)
					]
#endif

					// Bake By Actor Checkbox
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 2, 0, 2)
					.VAlign(VAlign_Center)
					[
						SNew(SCheckBox)
						.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
							{
								bBakeByActor = State == ECheckBoxState::Checked;
								SaveUISetting();
							})
						.IsChecked(MakeAttributeLambda([=, this] {return bBakeByActor ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }))
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().ToggleBakeByActor))
 						.Content()
 						[
							SNew(STextBlock)
							.Text(this, &SPivotToolWidget::GetMatchActorHeaderText)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.CheckboxText")
							.Justification(ETextJustify::Center)
						]
					]
#endif

					// Bake by Actor Loc
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ContentPadding(ButtonContentPadding)
						.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
						.OnClicked(this, &SPivotToolWidget::OnBakeByActorLocClicked)
						.IsEnabled(this, &SPivotToolWidget::IsBakeByActorEnabled)
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().BakeByActorLoc))
						[
							SNew(STextBlock)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
							.Text(LOCTEXT("BakeLoc", "Bake Loc"))
							.MinDesiredWidth(ButtonTextMinDesiredWidth)
							.Justification(ETextJustify::Center)
						]
					]
#endif

					// Rotation
#if PIVOTTOOL_FOLD
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ContentPadding(ButtonContentPadding)
						.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
						.OnClicked(this, &SPivotToolWidget::OnBakeByActorRotClicked)
						.IsEnabled(this, &SPivotToolWidget::IsBakeByActorEnabled)
						.ToolTip(FPivotToolCommands::Get().CreateToolTip(FPivotToolCommands::Get().BakeByActorRot))
						[
							SNew(STextBlock)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
							.Text(LOCTEXT("BakeRot", "Bake Rot"))
							.MinDesiredWidth(ButtonTextMinDesiredWidth)
							.Justification(ETextJustify::Center)
						]
					]
#endif
				]
			] // Bake by Actor
#endif

			VERTICAL_SPACER

			// Version
#if PIVOTTOOL_FOLD
			+ SVerticalBox::Slot()
			.Padding(0, VerticalPadding * 2)
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::ScaleToFit)
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.Padding(2, 0)
					.AutoWidth()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.Padding(0)
						.AutoHeight()
						[
							SNew(SImage)
							.Image(FPivotToolStyle::Get().GetBrush("PivotTool.PivotToolText"))
						]
					]

					+ SHorizontalBox::Slot()
					.Padding(2, 0)
					.AutoWidth()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(VersionName)
						.TextStyle(FPivotToolStyle::Get(), "PivotTool.VersionText")
					]

					+ SHorizontalBox::Slot()
					.Padding(6, 0)
					.AutoWidth()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.ContentPadding(0)
						.ButtonStyle(FPivotToolStyle::Get(), "PivotTool.HyperLinkButton.Normal")
						.OnClicked(FOnClicked::CreateLambda([this] { this->bShowChangelog = !this->bShowChangelog; return FReply::Handled(); }))
						[
							SNew(STextBlock)
							.TextStyle(FPivotToolStyle::Get(), "PivotTool.Property.LinkText.Small")
							.Text(LOCTEXT("ChangeLog", "Change Log"))
						]
					]
				]
			]
#endif

		]
#endif	// Pivot UI <<

		]);		
}

bool SPivotToolWidget::SupportsKeyboardFocus() const
{
	return true;
}

FReply SPivotToolWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = FReply::Unhandled();
	{
		if (InKeyEvent.GetKey() == EKeys::LeftControl || InKeyEvent.GetKey() == EKeys::RightControl)
		{
			bIsCtrlDown = true;
			Reply = FReply::Handled();
		}
		if (InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::RightShift)
		{
			bIsShiftDown = true;
			Reply = FReply::Handled();
		}
		if (InKeyEvent.GetKey() == EKeys::LeftAlt || InKeyEvent.GetKey() == EKeys::RightAlt)
		{
			bIsAltDown = true;
			Reply = FReply::Handled();
		}
	}
	return Reply;
}

FReply SPivotToolWidget::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = FReply::Unhandled();

	//if (IsEnabled())
	{
		if (InKeyEvent.GetKey() == EKeys::LeftControl || InKeyEvent.GetKey() == EKeys::RightControl)
		{
			bIsCtrlDown = false;
			Reply = FReply::Handled();
//			bCanToggleAlternativeButton = true;
		}
		if (InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::RightShift)
		{
			bIsShiftDown = false;
			Reply = FReply::Handled();
			//Invalidate(EInvalidateWidget::Layout);
		}
		if (InKeyEvent.GetKey() == EKeys::LeftAlt || InKeyEvent.GetKey() == EKeys::RightAlt)
		{
			bIsAltDown = false;
			Reply = FReply::Handled();
			//Invalidate(EInvalidateWidget::Layout);
		}
	}

	return Reply;
}

void SPivotToolWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	TickPreviewPivots();

	if (WidthLastFrame != AllottedGeometry.Size.X)
	{
		WidthLastFrame = AllottedGeometry.Size.X;
		TickUIWrapping();
	}
}

void SPivotToolWidget::SaveUISetting()
{
	GConfig->SetBool(TEXT("PivotTool"), TEXT("bDisplayAlignActors"), bDisplayAlignActors, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("PivotTool"), TEXT("bDisplayMatchActors"), bDisplayMatchActors, GEditorPerProjectIni);

	GConfig->SetBool(TEXT("PivotTool"), TEXT("bFreezeRotation"), bFreezeRotation, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("PivotTool"), TEXT("bFreezeScale"), bFreezeScale, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("PivotTool"), TEXT("bKeepActorsInPlace"), bKeepActorsInPlace, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("PivotTool"), TEXT("bCopyPivotInWorldSpace"), bCopyPivotInWorldSpace, GEditorPerProjectIni);

	GConfig->SetBool(TEXT("PivotTool"), TEXT("bBakeByActor"), bBakeByActor, GEditorPerProjectIni);
	GConfig->SetBool(TEXT("PivotTool"), TEXT("bBakeByActorDuplicate"), bBakeByActorDuplicate, GEditorPerProjectIni);
}

void SPivotToolWidget::LoadUISetting()
{
	GConfig->GetBool(TEXT("PivotTool"), TEXT("bDisplayAlignActors"), bDisplayAlignActors, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("PivotTool"), TEXT("bDisplayMatchActors"), bDisplayMatchActors, GEditorPerProjectIni);

	GConfig->GetBool(TEXT("PivotTool"), TEXT("bFreezeRotation"), bFreezeRotation, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("PivotTool"), TEXT("bFreezeScale"), bFreezeScale, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("PivotTool"), TEXT("bKeepActorsInPlace"), bKeepActorsInPlace, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("PivotTool"), TEXT("bCopyPivotInWorldSpace"), bCopyPivotInWorldSpace, GEditorPerProjectIni);

	GConfig->GetBool(TEXT("PivotTool"), TEXT("bBakeByActor"), bBakeByActor, GEditorPerProjectIni);
	GConfig->GetBool(TEXT("PivotTool"), TEXT("bBakeByActorDuplicate"), bBakeByActorDuplicate, GEditorPerProjectIni);
}

FText SPivotToolWidget::GetActorPivotHeaderText() const
{
	const bool bHideText = WidthLastFrame < PivotToolWidgetConstants::HideActorPivotTextWidth ? true : false;
	return bHideText ? LOCTEXT("ActorPivot.Tight:", "") : LOCTEXT("ActorPivot:", "Actor Pivot:");
}

FText SPivotToolWidget::GetMeshPivotHeaderText() const
{
	const bool bHideText = WidthLastFrame < PivotToolWidgetConstants::HideMeshPivotTextWidth ? true : false;
	return bHideText ? LOCTEXT("MeshPivot.Tight:", "") : LOCTEXT("MeshPivot:", "Mesh Pivot:");
}

FReply SPivotToolWidget::OnApplyOffsetClicked()
{
	ECoordSystem CoordSystem = GLevelEditorModeTools().GetCoordSystem();
	bool bWorldSpace = CoordSystem == COORD_World;
	FPivotToolUtil::OffsetSelectedActorsPivotOffset(PivotOffsetInput, bWorldSpace);
	return FReply::Handled();
}

bool SPivotToolWidget::IsApplyOffsetEnabled() const
{
	return !PivotOffsetInput.IsNearlyZero() && GEditor->GetSelectedActorCount() > 0;
}

FReply SPivotToolWidget::OnSnapToVertexClicked()
{
	FPivotToolUtil::SnapSelectedActorsPivotToVertex();
	return FReply::Handled();
}

bool SPivotToolWidget::IsSnapToVertexEnabled() const
{
	return GEditor->GetSelectedActorCount() > 0;
}

FReply SPivotToolWidget::OnSetClicked()
{
	FEditorModeTools& EditorModeTools = GLevelEditorModeTools();
	FPivotToolUtil::UpdateSelectedActorsPivotOffset(EditorModeTools.PivotLocation, /*bWorldSpace=*/ true);
	return FReply::Handled();
}

FReply SPivotToolWidget::OnCopyClicked()
{	
	FPivotToolUtil::CopyActorPivotOffset(bCopyPivotInWorldSpace);
	return FReply::Handled();
}

FReply SPivotToolWidget::OnPasteClicked()
{
	FPivotToolUtil::PasteActorPivotOffset(bCopyPivotInWorldSpace);
	return FReply::Handled();
}

bool SPivotToolWidget::IsPasteEnabled() const
{
	return FPivotToolUtil::HasPivotOffsetCopied();
}

FReply SPivotToolWidget::OnResetClicked()
{
	const bool bWorldSpace = false;
	FPivotToolUtil::UpdateSelectedActorsPivotOffset(FVector::ZeroVector, bWorldSpace);
	return FReply::Handled();
}

FReply SPivotToolWidget::OnBakeClicked()
{
	const bool bDuplicate = false;

	USelection* SelectedActors = GEditor->GetSelectedActors();
	int32 SucceedCount = 0;
	TArray<UStaticMesh*> BakedMeshes;

	FPivotToolOptions Options;
	UpdateOptions(Options);

	AActor* LastSelectedActor = GetLastSelectedActor();
	if (bBakeByActor && LastSelectedActor == NULL)
	{
		FPivotToolUtil::NotifyMessage(FText::FromString(FString::Printf(TEXT("At least 2 actors needed to bake by last selected actor!"))), 1.f);
		return FReply::Handled();
	}

	bool bSilentMode = SelectedActors->Num() > 1;
	for (FSelectionIterator SelectionIt(*SelectedActors); SelectionIt; ++SelectionIt)
	{
		if (AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(*SelectionIt))
		{
			if (bBakeByActor && LastSelectedActor == StaticMeshActor)
			{
				continue;
			}

			if (UStaticMesh* StaticMesh = StaticMeshActor->GetStaticMeshComponent()->GetStaticMesh())
			{
				if (BakedMeshes.Find(StaticMesh) == INDEX_NONE)
				{
					if (FPivotToolUtil::BakeStaticMeshActorsPivotOffsetToRawMesh(StaticMeshActor, Options, /*bDuplicate=*/ bDuplicate, /*bSilentMode=*/ bSilentMode))
					{
						++SucceedCount;
					}
					BakedMeshes.AddUnique(StaticMesh);
				}
				else
				{
					// Reset Pivot Offset if Mesh Pivot already baked
					FPivotToolUtil::UpdateActorPivotOffset(StaticMeshActor, FVector::ZeroVector, /*bWorldSpace=*/ false);
				}
			}
		}
	}

	FPivotToolUtil::NotifyMessage(FText::FromString(FString::Printf(TEXT("%d Static Mesh Baked!"), SucceedCount)), 0.5f);

	return FReply::Handled();
}

FReply SPivotToolWidget::OnDuplicateAndBakeClicked()
{
	const bool bDuplicate = true;

	FPivotToolOptions Options;
	UpdateOptions(Options);

	USelection* SelectedActors = GEditor->GetSelectedActors();
	int32 SucceedCount = 0;
	for (FSelectionIterator SelectionIt(*SelectedActors); SelectionIt; ++SelectionIt)
	{
		AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(*SelectionIt);
		if (StaticMeshActor)
		{
			if (FPivotToolUtil::BakeStaticMeshActorsPivotOffsetToRawMesh(StaticMeshActor, Options, /*bDuplicate=*/ bDuplicate))
			{
				++SucceedCount;
			}
		}
	}

	FPivotToolUtil::NotifyMessage(FText::FromString(FString::Printf(TEXT("%d Static Mesh Actor Baked!"), SucceedCount)), 0.5f);

	return FReply::Handled();
}

bool SPivotToolWidget::IsBakeEnabled() const
{
	if (bBakeByActor)
		return false;

	AStaticMeshActor* StaticMeshActor = GEditor->GetSelectedActors()->GetBottom<AStaticMeshActor>();
	return StaticMeshActor != NULL;
}


FReply SPivotToolWidget::OnBakeByActorLocClicked()
{
	bBakeByActorLoc = true;
	bBakeByActorRot = false;
	return OnBakeClicked();
}

FReply SPivotToolWidget::OnBakeByActorRotClicked()
{
	bBakeByActorLoc = false;
	bBakeByActorRot = true;
	return OnBakeClicked();
}


FText SPivotToolWidget::GetAlignActorsCheckboxText() const
{
	if (bDisplayAlignActors)
		return LOCTEXT(" < ", "< ");

	return WidthLastFrame > 418.f ? LOCTEXT(" > ", " > ") : LOCTEXT(" > ", " > ");
}

bool SPivotToolWidget::IsBakeByActorEnabled() const
{
	if (bBakeByActor)
	{
		AActor* LastSelected = GetLastSelectedActor();
		return LastSelected != NULL;
	}
	return false;
}

FText SPivotToolWidget::GetFreezeRotationCheckboxText() const
{
	return WidthLastFrame > 418.f ? LOCTEXT("FreezeRotaion", "Freeze Rotation") : LOCTEXT("FreezeRotaion.Tight", "Rot");
}

FText SPivotToolWidget::GetFreezeScaleCheckboxText() const
{
	return WidthLastFrame > 418.f ? LOCTEXT("bFreezeScale", "Freeze Scale") : LOCTEXT("bFreezeScale.Tight", "Scale");
}

FText SPivotToolWidget::GetKeepActorsInPlaceCheckboxText() const
{
	return WidthLastFrame > 418.f ? LOCTEXT("bKeepActorsInPlace", "Keep in place") : LOCTEXT("bFreezeScale.Tight", "InPlace");
}


FText SPivotToolWidget::GetDisplayMatchActorCheckboxText() const
{
	if (bDisplayMatchActors)
		return LOCTEXT(" < ", "< ");
	else
		return LOCTEXT(" > ", " > ");
}


FText SPivotToolWidget::GetMatchActorHeaderText() const
{
	const bool bHideText = WidthLastFrame < PivotToolWidgetConstants::HideMeshPivotTextWidth ? true : false;
	return bHideText ? LOCTEXT("BakeByActors.Tight:", "") : LOCTEXT("BakeByActors:", "Match Actor:");
}

AActor* SPivotToolWidget::GetLastSelectedActor() const
{
	TArray<AActor*> SelectedActors;
	GEditor->GetSelectedActors()->GetSelectedObjects<AActor>(SelectedActors);

	if (SelectedActors.Num() > 1) // at least 2
	{
		AActor* LastSelectedActor = SelectedActors.Last();
		if (LastSelectedActor != NULL)
		{
			return LastSelectedActor;
		}
	}
	
	return NULL;
}


FText SPivotToolWidget::GetCopyInWorldSpaceCheckboxText() const
{
	return WidthLastFrame > 418.f ? LOCTEXT("WorldSpace", "World Space") : LOCTEXT("WorldSpace.Tight", "World");
}

FReply SPivotToolWidget::OnPivotPresetSelected(EPivotPreset InPreset)
{
	PIVOTTOOL_LOG(Display, TEXT("SPivotToolWidget::OnPivotPresetSelected: %d"), (int32)InPreset);

	FPivotToolOptions Options;
	UpdateOptions(Options);
	FPivotToolUtil::UpdateSelectedActorsPivotOffset(InPreset, Options);

	PreviewPivots.Empty();

	return FReply::Handled();
}

void SPivotToolWidget::OnPivotPresetHovered(EPivotPreset InPreset)
{
	PIVOTTOOL_LOG(Display, TEXT("SPivotToolWidget::OnPivotPresetHovered: %d"), (int32)InPreset);

	FPivotToolOptions Options;
	UpdateOptions(Options);

	FPivotToolUtil::GetPreviewPivotsOfSelectedActors(InPreset, Options, PreviewPivots);
}

void SPivotToolWidget::OnPivotPresetUnhovered(EPivotPreset InPreset)
{
	PIVOTTOOL_LOG(Display, TEXT("SPivotToolWidget::OnPivotPresetUnhovered: %d"), (int32)InPreset);

	PreviewPivots.Empty();
}

void SPivotToolWidget::OnSetPivotOffsetInput(float InValue, int32 InAxis)
{
	if (InAxis == 0)
	{
		PivotOffsetInput.X = InValue;
	}
	else if (InAxis == 1)
	{
		PivotOffsetInput.Y = InValue;
	}
	else if (InAxis == 2)
	{
		PivotOffsetInput.Z = InValue;
	}
}


void SPivotToolWidget::TickPreviewPivots()
{
	const UPivotToolSettings* PivotToolSettings = GetDefault<UPivotToolSettings>();

	if (!PivotToolSettings->bDisplayPivotPresetPreview)
	{
		return;
	}

	if (GEditor->GetSelectedActorCount() <= 0)
	{
		return;
	}

	if (PreviewPivots.Num() <= 0)
	{
		return;
	}

	if (AActor* LastSelectedActor = GEditor->GetSelectedActors()->GetBottom<AActor>())
	{
		if (UWorld* World = LastSelectedActor->GetWorld())
		{
			EPivotToolPreviewShape PreviewShape = PivotToolSettings->PivotPresetPreviewShape;
			const FColor PreviewColor = PivotToolSettings->PivotPresetPreviewColor;

			FVector ActorOrigin, ActorExtent;
			LastSelectedActor->GetActorBounds(/*bOnlyCollidingComponents*/false, ActorOrigin, ActorExtent);
			float Radius = FMath::Max3(ActorExtent.X, ActorExtent.Y, ActorExtent.Z) / 20.f;

			const float LineThickness = Radius / 10.f;			
			const int32 SphereSegments = 12;
			const float CoordScale = Radius * 2;

			const bool bPersistent = false;
			const float LifeTime = -1.f;
			uint8 DepthPriority = SDPG_Foreground;
			const float PointSize = 10.f;

			for (auto It = PreviewPivots.CreateConstIterator(); It; ++It)
			{
				const FTransform& Pivot = *It;

				if (PreviewShape == EPivotToolPreviewShape::Sphere)
				{
					DrawDebugSphere(World, Pivot.GetLocation(), Radius, SphereSegments, PreviewColor, bPersistent, LifeTime, DepthPriority, LineThickness);
				}
				else if (PreviewShape == EPivotToolPreviewShape::Point)
				{
					DrawDebugPoint(World, Pivot.GetLocation(), PointSize, PreviewColor, bPersistent, LifeTime, DepthPriority);
				}

				if (PreviewShape != EPivotToolPreviewShape::None)
				{
					DrawDebugCoordinateSystem(World, Pivot.GetLocation(), Pivot.Rotator(), CoordScale, bPersistent, LifeTime, DepthPriority, LineThickness);
				}
			}
		}
	}
}

void SPivotToolWidget::UpdateOptions(FPivotToolOptions& Options)
{
	Options.bAutoSave = PivotPreset->IsAutoSave() == ECheckBoxState::Checked;
	Options.bGroupMode = PivotPreset->IsGroupMode() == ECheckBoxState::Checked;
	Options.bLastSelectedMode = PivotPreset->IsLastSelectedMode() == ECheckBoxState::Checked;
	Options.bVertexMode = PivotPreset->IsVertexMode() == ECheckBoxState::Checked;
	Options.bChildrenMode = PivotPreset->IsChildrenMode() == ECheckBoxState::Checked;

	Options.bFreezeRotation = bFreezeRotation;
	Options.bFreezeScale = bFreezeScale;
	Options.bKeepActorsInPlace = bKeepActorsInPlace;

	Options.bBakeByActor = bBakeByActor;

	if (Options.bBakeByActor)
	{
		Options.bBakeByActorLoc = bBakeByActorLoc;
		Options.bFreezeRotation = bBakeByActorRot;
		Options.bFreezeScale = false;
		Options.bKeepActorsInPlace = true;

		if (AActor* LastSelected = GetLastSelectedActor())
		{
			Options.BakeByActorTransform = LastSelected->GetActorTransform();
		}
	}
}

TSharedRef<SWidget> SPivotToolWidget::CreateChangeLogWidget()
{
 	FString AboutLinkRoot(TEXT("Shared/About"));
 	FDocumentationStyle DocumentationStyle = FExtDocumentationStyle::GetChangLogDocumentationStyle();
 	FPivotToolModule& PivotToolModule = FModuleManager::LoadModuleChecked<FPivotToolModule>("PivotTool");
 	AboutPage = PivotToolModule.GetDocumentation()->GetPage(AboutLinkRoot, /*TSharedPtr<FParserConfiguration>()*/NULL, DocumentationStyle);
 	FExcerpt ChangeLogExcerpt;
 	AboutPage->GetExcerpt(TEXT("ChangeLog"), ChangeLogExcerpt);
 	AboutPage->GetExcerptContent(ChangeLogExcerpt);
 	TSharedRef<SWidget> ChangeLogContentWidget = ChangeLogExcerpt.Content.IsValid() ? ChangeLogExcerpt.Content.ToSharedRef() : SNew(SSpacer);

	return SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f))
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(6.f)
		.Visibility(MakeAttributeLambda([=, this] {return bShowChangelog ? EVisibility::Visible : EVisibility::Collapsed; }))
		[
			SNew(SVerticalBox)

			// Header
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 4, 0, 8).HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.TextStyle(FPivotToolStyle::Get(), "PivotTool.ChangeLogHeaderText")
				.Text(LOCTEXT("PivotToolChangeLog", "Pivot Tool Change Log"))
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 8, 0).HAlign(HAlign_Right)
			[
				SNew(SButton)
				.ButtonStyle(&FPivotToolStyle::Get().GetWidgetStyle<FButtonStyle>("PivotTool.ButtonStyle.Round.Black"))
				.TextStyle(FPivotToolStyle::Get(), "PivotTool.ButtonText")
				.Text(LOCTEXT("Close", "Close"))
				.HAlign(HAlign_Center)
				.OnClicked(FOnClicked::CreateLambda([this] { this->bShowChangelog = false; return FReply::Handled(); }))
			]

			// Change Log
			+ SVerticalBox::Slot().Padding(0, 2, 0, 2).FillHeight(1.0f).HAlign(HAlign_Center)
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.3f, 0.3f, 0.3f))
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(1.f)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot().Padding(5, 5)
					[
						ChangeLogContentWidget
					]
				]
			]
		];
}

void SPivotToolWidget::TickUIWrapping()
{
	ApplyOffsetWrapBox->SetWrapSize(WidthLastFrame);
	//MainToolboxWrapBox->SetWrapSize(WidthLastFrame);
	AlignActorsWidget->SetWrapWidth(WidthLastFrame);

	PIVOTTOOL_LOG(Display, TEXT("SPivotToolWidget::TickUIWrapping: %.2f"), WidthLastFrame)
}

#undef HORIZONTAL_SPACER
#undef VERTICAL_SPACER

#undef LOCTEXT_NAMESPACE
