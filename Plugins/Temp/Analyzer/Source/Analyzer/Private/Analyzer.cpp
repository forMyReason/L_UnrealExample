// Copyright Epic Games, Inc. All Rights Reserved.

#include "Analyzer.h"
#include "AnalyzerStyle.h"
#include "AnalyzerCommands.h"
#include "Misc/MessageDialog.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "ToolMenus.h"

static const FName AnalyzerTabName("Analyzer");

#define LOCTEXT_NAMESPACE "FAnalyzerModule"

void FAnalyzerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FAnalyzerStyle::Initialize();
	FAnalyzerStyle::ReloadTextures();

	FAnalyzerCommands::Register();
	
	AnalyzerCommands = MakeShareable(new FUICommandList);

	AnalyzerCommands->MapAction(
		FAnalyzerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FAnalyzerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAnalyzerModule::RegisterMenus));
}

void FAnalyzerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FAnalyzerStyle::Shutdown();

	FAnalyzerCommands::Unregister();
}

// StaticMeshAnalyzer.cpp

FString GetLODInfo(const UStaticMesh* StaticMesh)
{
	FString LODInfo;
	const int32 NumLODs = StaticMesh->GetNumLODs();
    
	for (int32 LODIndex = 0; LODIndex < NumLODs; ++LODIndex)
	{
		if (LODIndex > 0)
		{
			LODInfo += TEXT("|");
		}
		LODInfo += FString::Printf(TEXT("LOD%d: %d tris|ScreenSize: %.2f"),
			LODIndex,
			StaticMesh->GetNumTriangles(LODIndex),
			0.0f);
	}
    
	return TEXT("\"") + LODInfo + TEXT("\""); // 用引号括起以避免CSV解析问题
}

FString GetMaterialInfo(const UStaticMesh* StaticMesh)
{
	FString MaterialInfo;
	const TArray<FStaticMaterial>& StaticMaterials = StaticMesh->GetStaticMaterials();
    
	for (int32 i = 0; i < StaticMaterials.Num(); ++i)
	{
		if (i > 0)
		{
			MaterialInfo += TEXT("|");
		}
		UMaterialInterface* Material = StaticMaterials[i].MaterialInterface;
		MaterialInfo += Material ? Material->GetName() : TEXT("None");
	}
    
	return TEXT("\"") + MaterialInfo + TEXT("\"");
}

FString GetUVInfo(const UStaticMesh* StaticMesh)
{
	FString UVInfo;
	if (StaticMesh->GetRenderData() && StaticMesh->GetRenderData()->LODResources.Num() > 0)
	{
		int32 NumUVChannels = StaticMesh->GetRenderData()->LODResources[0].GetNumTexCoords();
		UVInfo = FString::FromInt(NumUVChannels);
	}
	else
	{
		UVInfo = TEXT("0");
	}
    
	return UVInfo;
}

FString GetCollisionInfo(const UStaticMesh* StaticMesh)
{
	// 获取碰撞三角面数（需要遍历所有碰撞元素）
	int32 CollisionTriCount = 0;
	// if (StaticMesh->GetBodySetup())
	// {
	// 	for (const auto&Elem : StaticMesh->GetBodySetup()->AggGeom.ConvexElems)
	// 	{
	// 		CollisionTriCount += Elem.GetTriangles().Num() / 3;
	// 	}
	// }
    
	return FString::FromInt(CollisionTriCount);
}

TArray<FString> GetStaticMeshInfo(UStaticMesh* StaticMesh)
{
	TArray<FString> Info;
    
	// 基础信息
	Info.Add(StaticMesh->GetName());
	Info.Add(FString::FromInt(StaticMesh->GetNumVertices(0))); // LOD0顶点数
	Info.Add(FString::FromInt(StaticMesh->GetNumTriangles(0))); // LOD0三角形数
    
	// LOD信息
	Info.Add(FString::FromInt(StaticMesh->GetNumLODs()));
	Info.Add(GetLODInfo(StaticMesh));
    
	// 材质信息
	Info.Add(FString::FromInt(StaticMesh->GetStaticMaterials().Num()));
	Info.Add(GetMaterialInfo(StaticMesh));
    
	// UV信息
	Info.Add(GetUVInfo(StaticMesh));
    
	// 光照信息
	Info.Add(FString::FromInt(StaticMesh->GetLightMapResolution()));
	Info.Add(StaticMesh->bGenerateMeshDistanceField ? TEXT("Yes") : TEXT("No"));
    
	// CPU访问
	Info.Add(StaticMesh->bAllowCPUAccess ? TEXT("Yes") : TEXT("No"));
    
	// 碰撞信息
	Info.Add(GetCollisionInfo(StaticMesh));
    
	// 预估资源大小
	Info.Add(FString::SanitizeFloat(StaticMesh->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal) / 1024.0f));
    
	return Info;
}

bool ExportStaticMeshInfoToCSV(const FString& OutputPath, const FString& MeshDirectory)
{
    TArray<FString> CSVLines;
    // 添加CSV表头
    CSVLines.Add(TEXT("Mesh Name,Vertex Count,Triangle Count,LOD Count,LOD Info,Material Slots,Material Names,UV Channels,Lightmap Resolution,Generates Distance Field,Allow CPU Access,Collision Tris,Estimated Size (KB)"));
    
    // 加载资源注册模块
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
    
    // 查找所有Static Mesh资源
    TArray<FAssetData> AssetData;
    FARFilter Filter;
    Filter.PackagePaths.Add(*MeshDirectory);
    Filter.ClassPaths.Add(UStaticMesh::StaticClass()->GetClassPathName());
    AssetRegistry.GetAssets(Filter, AssetData);
    
    // 遍历所有找到的Static Mesh
    for (const FAssetData& Data : AssetData)
    {
        UStaticMesh* StaticMesh = Cast<UStaticMesh>(Data.GetAsset());
        if (StaticMesh)
        {
            TArray<FString> MeshInfo = GetStaticMeshInfo(StaticMesh);
            CSVLines.Add(FString::Join(MeshInfo, TEXT(",")));
        }
    }
    
    // 写入CSV文件
    return FFileHelper::SaveStringArrayToFile(CSVLines, *OutputPath);
}

void FAnalyzerModule::PluginButtonClicked()
{
	ExportStaticMeshInfoToCSV(TEXT("C:/Users/79160/Desktop/glb/monkey.csv"),
		TEXT("/Game/StarterContent/Props/"));
	UE_LOG(LogTemp, Warning, TEXT("Analyzer Module Button Clicked"))
}

void FAnalyzerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FAnalyzerCommands::Get().PluginAction, AnalyzerCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAnalyzerCommands::Get().PluginAction));
				Entry.SetCommandList(AnalyzerCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAnalyzerModule, Analyzer)