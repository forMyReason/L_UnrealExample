// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MeshCombinerStyle.h"

class FMeshCombinerCommands : public TCommands<FMeshCombinerCommands>
{
public:

	FMeshCombinerCommands()
		: TCommands<FMeshCombinerCommands>(TEXT("MeshCombiner"), NSLOCTEXT("Contexts", "MeshCombiner", "MeshCombiner Plugin"), NAME_None, FMeshCombinerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
