// Copyright 2017-2021 marynate. All Rights Reserved.

#include "PivotToolCommands.h"
#include "PivotTool.h"

#define LOCTEXT_NAMESPACE "PivotToolCommands"

#define UI_COMMAND_WITH_TOOLTIP( CommandId, FriendlyName, InDescription, InLink, CommandType, InDefaultChord, ... ) \
	UI_COMMAND( CommandId, FriendlyName, InDescription, CommandType, InDefaultChord, ## __VA_ARGS__ ); \
	AddFullTooltip(CommandId, InLink);

void FPivotToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPivotTool, "PivotTool", "Bring up PivotTool window", EUserInterfaceActionType::Button, FInputChord());

	FString CommandsLink(TEXT("Shared/Commands"));

	UI_COMMAND(ApplyPivotOffset, "Apply Pivot Offset", "Apply offset to pivot", EUserInterfaceActionType::Button, FInputChord());
	//UI_COMMAND_WITH_TOOLTIP(ApplyPivotOffset, "Apply Pivot Offset", "Apply offset to pivot", CommandsLink, EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SnapPivotToVert, "Snap Pivot to Vert", "Snap pivot to closest vertex postion", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SetActorPivot, "SetActorPivot", "Save temporary actor pivot placement (Alt + Middle mouse)", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ResetActorPivot, "ResetActorPivot", "Reset actor pivot", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CopyActorPivot, "CopyActorPivot", "Copy actor pivot", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(PasteActorPivot, "PasteActorPivot", "Paste actor pivot", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ToggleCopyActorPivotInWorldSpace, "ToggleCopyActorPivotInWorldSpace", "Copy pivot in world space?", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(BakeMeshPivot, "BakeMeshPivot", "Bake last selected static mesh actor's pivot to static mesh.\nYou can not undo this operation!", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DuplicateBakeMeshPivot, "DuplicateBakeMeshPivot", "Duplicate static mesh and bake last selected static mesh actor's pivot to duplicated static mesh.\nYou can not undo this operation!", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ToggleBakeFreezeRotation, "ToggleBakeFreezeRotation", "Freeze rotation when baking", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(ToggleBakeFreezeScale, "ToggleBakeFreezeScale", "Freeze scale when baking", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(ToggleBakeInPlace, "ToggleBakeInPlace", "Try to keep actors in the level referencing same StaticMesh in place", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(ToggleBakeByActor, "ToggleBakeByActor", "Bake mesh pivot to match last selected actor", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(BakeByActorLoc, "BakeByActorLoc", "Bake mesh pivot to match last selected actor's location", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(BakeByActorRot, "BakeByActorRot", "Bake mesh pivot to match last selected actor's rotation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AlignActorsAll, "AlignActorsAll", "Align selected actors to last selected actor's location", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AlignActorsX, "AlignActorsX", "Align selected actors' max X location", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AlignActorsNegX, "AlignActorsNegX", "Align selected actors' min X location", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AlignActorsY, "AlignActorsY", "Align selected actors' max Y location", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AlignActorsNegY, "AlignActorsNegY", " selected actors' min Y location", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AlignActorsZ, "AlignActorsZ", "Align selected actors' max Z location", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AlignActorsNegZ, "AlignActorsNegZ", "Align selected actors' min Z location", EUserInterfaceActionType::Button, FInputChord());
}

TSharedRef<class SToolTip> FPivotToolCommands::CreateToolTip(const TSharedPtr<FUICommandInfo>& InUICommandInfo, bool bAlwaysShowFullTooltip /*= false*/) const
{
	FPivotToolModule& PivotToolModule = FModuleManager::LoadModuleChecked<FPivotToolModule>("PivotTool");
	if (const FPivotToolCommandTooltip* CommandTooltip = CommandTooltips.Find(InUICommandInfo.Get()))
	{
		if (bAlwaysShowFullTooltip)
		{
			TSharedPtr< SToolTip > ToolTip = SNew(SToolTip);
			return PivotToolModule.GetDocumentation()->CreateToolTip(CommandTooltip->BasicTooltip, /*hack:*/ToolTip, CommandTooltip->Link, CommandTooltip->ExcerptName);
		}
		else
		{
			return PivotToolModule.GetDocumentation()->CreateToolTip(CommandTooltip->BasicTooltip, nullptr, CommandTooltip->Link, CommandTooltip->ExcerptName);
		}
	}
	else
	{
		return SNew(SToolTip)
			.BorderImage(FCoreStyle::Get().GetBrush("ToolTip.BrightBackground"))
			.Text(FText::FromString(GetBasicToolTip(InUICommandInfo)));
	}
}

bool FPivotToolCommands::HasFullTooltip(const TSharedPtr<FUICommandInfo>& InCommand) const
{
	return nullptr != CommandTooltips.Find(InCommand.Get());
}

void FPivotToolCommands::AddFullTooltip(const TSharedPtr<FUICommandInfo>& InCommand, const FString& InLink)
{
	CommandTooltips.Add(InCommand.Get(), FPivotToolCommandTooltip(GetBasicToolTip(InCommand), InLink, /*InExcerptName*/InCommand->GetCommandName().ToString()));
}

FString FPivotToolCommands::GetBasicToolTip(const TSharedPtr<class FUICommandInfo>& CommandInfo) const
{
	const FString CommandDescription = CommandInfo->GetDescription().ToString();
	const FString CommandInputText = CommandInfo->GetFirstValidChord()->GetInputText().ToString();

	return CommandInputText.IsEmpty()
		? FString::Printf(TEXT("%s"), *CommandDescription)
		: FString::Printf(TEXT("%s (%s)"), *CommandDescription, *CommandInputText);
}

#undef UI_COMMAND_WITH_TOOLTIP
#undef LOCTEXT_NAMESPACE
