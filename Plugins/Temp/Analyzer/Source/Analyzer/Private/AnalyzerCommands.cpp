// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnalyzerCommands.h"

#define LOCTEXT_NAMESPACE "FAnalyzerModule"

void FAnalyzerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Analyzer", "Execute Analyzer action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
