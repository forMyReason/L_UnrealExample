// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshCombiner.h"
#include "MeshCombinerStyle.h"
#include "MeshCombinerCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "MeshCombiner/Public/MergeModel.h"

static const FName MeshCombinerTabName("MeshCombiner");

#define LOCTEXT_NAMESPACE "FMeshCombinerModule"

void FMeshCombinerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMeshCombinerStyle::Initialize();
	FMeshCombinerStyle::ReloadTextures();

	FMeshCombinerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMeshCombinerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMeshCombinerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMeshCombinerModule::RegisterMenus));
}

void FMeshCombinerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMeshCombinerStyle::Shutdown();

	FMeshCombinerCommands::Unregister();
}

void FMeshCombinerModule::PluginButtonClicked()
{
}

void FMeshCombinerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMeshCombinerCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMeshCombinerCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMeshCombinerModule, MeshCombiner)