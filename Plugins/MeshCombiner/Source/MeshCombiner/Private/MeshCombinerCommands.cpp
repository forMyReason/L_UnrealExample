// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshCombinerCommands.h"

#define LOCTEXT_NAMESPACE "FMeshCombinerModule"

void FMeshCombinerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MeshCombiner", "Execute MeshCombiner action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
