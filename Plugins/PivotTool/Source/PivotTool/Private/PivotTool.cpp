// Copyright 2017-2021 marynate. All Rights Reserved.

#include "PivotTool.h"
#include "PivotToolStyle.h"
#include "PivotToolCommands.h"
#include "PivotToolEdMode.h"
#include "PivotToolSettings.h"

// Documentation
#include "DocumentationStyle.h"

#include "Widgets/SPivotToolWidget.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "LevelEditor.h"
#include "ISettingsModule.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

static const FName PivotToolTabName("PivotTool");

#define LOCTEXT_NAMESPACE "FPivotToolModule"

void FPivotToolModule::StartupModule()
{
	FPivotToolStyle::Initialize();
	FPivotToolStyle::ReloadTextures();

	// Documentation
	FExtDocumentationStyle::Initialize();
	Documentation = FExtDocumentation::Create();
	FDocumentationProvider::Get().RegisterProvider(*DocumentationHostPluginName, this);

	FPivotToolCommands::Register();
	ToolCommands = MakeShareable(new FUICommandList);
	ToolCommands->MapAction(
		FPivotToolCommands::Get().OpenPivotTool,
		FExecuteAction::CreateRaw(this, &FPivotToolModule::OpenPivotToolTab),
		FCanExecuteAction());
	
	RegisterMenu();

	// Register Settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Editor", "ContentEditors", "PivotTool",
			LOCTEXT("PivotToolEditorSettingsName", "Pivot Tool"),
			LOCTEXT("PivotToolEditorSettingsDescription", "Configure the options of the Pivot Tool."),
			GetMutableDefault<UPivotToolSettings>());
	}
}

void FPivotToolModule::ShutdownModule()
{
	FPivotToolStyle::Shutdown();
	FExtDocumentationStyle::Shutdown();

	FPivotToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PivotToolTabName);

	// Unregister Settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "ContentEditors", "PivotTool");
	}
}

TSharedRef<SDockTab> FPivotToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SAssignNew(ToolWindow, SPivotToolWidget)
		];
}

void FPivotToolModule::OpenPivotToolTab()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetLevelEditorTabManager()->TryInvokeTab(PivotToolTabName);
}

TSharedRef<IDocumentation> FPivotToolModule::GetDocumentation() const
{
	return Documentation.ToSharedRef();
}

void FPivotToolModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FPivotToolCommands::Get().OpenPivotTool);
}

void FPivotToolModule::RegisterMenu()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, ToolCommands, FToolBarExtensionDelegate::CreateRaw(this, &FPivotToolModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	struct MenuGroupHelper
	{
		static TSharedRef<FWorkspaceItem> FindOrCreateGroup(const FText& InGroupName)
		{
			TSharedRef<FWorkspaceItem> MenuRoot = WorkspaceMenu::GetMenuStructure().GetStructureRoot();
			const TArray<TSharedRef<FWorkspaceItem>>& ChildItems = MenuRoot->GetChildItems();
			for (const TSharedRef<FWorkspaceItem>& ChildItem : ChildItems)
			{
				if (ChildItem->GetDisplayName().EqualTo(InGroupName))
				{
					return ChildItem;
				}
			}
			return MenuRoot->AddGroup(InGroupName);;
		}
	};
	const FText NatesToolGroupName = LOCTEXT("NatesTools", "Nate's Tools");
	TSharedRef<FWorkspaceItem> NatesToolsMenuGroup = MenuGroupHelper::FindOrCreateGroup(NatesToolGroupName);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(PivotToolTabName, FOnSpawnTab::CreateRaw(this, &FPivotToolModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FPivotToolTabTitle", "Pivot Tool"))
		.SetMenuType(ETabSpawnerMenuType::Enabled)
		.SetIcon(FSlateIcon(FPivotToolStyle::GetStyleSetName(), "PivotTool.PivotToolMode.Small"))
		.SetGroup(NatesToolsMenuGroup);
}

void FPivotToolModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FPivotToolCommands::Get().OpenPivotTool);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPivotToolModule, PivotTool)
DEFINE_LOG_CATEGORY(LogPivotTool);