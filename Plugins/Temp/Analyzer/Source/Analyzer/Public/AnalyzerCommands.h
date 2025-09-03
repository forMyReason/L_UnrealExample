// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AnalyzerStyle.h"

class FAnalyzerCommands : public TCommands<FAnalyzerCommands>
{
public:

	FAnalyzerCommands()
		: TCommands<FAnalyzerCommands>(TEXT("Analyzer"), NSLOCTEXT("Contexts", "Analyzer", "Analyzer Plugin"), NAME_None, FAnalyzerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
