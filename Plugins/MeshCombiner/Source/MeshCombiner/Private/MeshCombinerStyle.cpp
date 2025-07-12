// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshCombinerStyle.h"
#include "MeshCombiner.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FMeshCombinerStyle::StyleInstance = nullptr;

void FMeshCombinerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMeshCombinerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FMeshCombinerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MeshCombinerStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FMeshCombinerStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MeshCombinerStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MeshCombiner")->GetBaseDir() / TEXT("Resources"));

	Style->Set("MeshCombiner.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FMeshCombinerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMeshCombinerStyle::Get()
{
	return *StyleInstance;
}
