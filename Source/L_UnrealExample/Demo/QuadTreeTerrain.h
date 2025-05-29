// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "QuadTreeTerrain.generated.h"

UCLASS()
class L_UNREALEXAMPLE_API AQuadTreeTerrain : public AActor
{
    GENERATED_BODY()
    
public:
    AQuadTreeTerrain();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnConstruction(const FTransform& Transform) override;

private:
    // 四叉树节点结构
    struct QuadTreeNode
    {
        FVector2D Center;      // 中心位置
        FVector2D Size;         // 节点尺寸
        int Depth = 0;          // 节点深度
        bool IsSubdivided = false; // 是否已细分
        
        // 四个子节点 (西北, 东北, 西南, 东南)
        TSharedPtr<QuadTreeNode> NW;
        TSharedPtr<QuadTreeNode> NE;
        TSharedPtr<QuadTreeNode> SW;
        TSharedPtr<QuadTreeNode> SE;
        
        // 该节点对应的Widget组件
        UWidgetComponent* WidgetComponent = nullptr;
        
        QuadTreeNode(const FVector2D& InCenter, const FVector2D& InSize, int InDepth)
            : Center(InCenter), Size(InSize), Depth(InDepth) {}
    };
    
    // 根节点
    TSharedPtr<QuadTreeNode> RootNode;
    
    // 构建四叉树
    void BuildQuadTree();
    
    // 递归细分节点
    void SubdivideNode(TSharedPtr<QuadTreeNode> Node, const FVector& CameraLocation);
    
    // 递归合并节点
    void MergeNodes(TSharedPtr<QuadTreeNode> Node, const FVector& CameraLocation);
    
    // 创建或更新Widget组件
    void CreateOrUpdateWidget(TSharedPtr<QuadTreeNode> Node);
    
    // 删除节点及其Widget
    void RemoveNodeAndWidget(TSharedPtr<QuadTreeNode> Node);
    
    // 绘制调试信息
    void DrawDebugQuadTree(TSharedPtr<QuadTreeNode> Node);
    
    // 获取摄像机位置
    FVector GetCameraLocation() const;
    
    // 计算节点到摄像机的距离
    float GetDistanceToCamera(const FVector2D& NodeCenter) const;
    
    // 计算摄像机到节点的距离是否小于细分阈值
    bool ShouldSubdivide(const QuadTreeNode& Node, const FVector& CameraLocation) const;
    
    // 计算节点是否应该合并
    bool ShouldMerge(const QuadTreeNode& Node, const FVector& CameraLocation) const;
    
    // 清除整个四叉树
    void ClearQuadTree();
    
public:
    // 配置参数
    UPROPERTY(EditAnywhere, Category = "QuadTree")
    FVector2D TerrainSize = FVector2D(2000.f, 2000.f); // 地形尺寸
    
    UPROPERTY(EditAnywhere, Category = "QuadTree", meta = (ClampMin = "1"))
    int MaxDepth = 5; // 最大细分深度
    
    UPROPERTY(EditAnywhere, Category = "QuadTree", meta = (ClampMin = "0.0"))
    float SubdivideDistanceFactor = 1000.f; // 细分距离因子
    
    UPROPERTY(EditAnywhere, Category = "QuadTree", meta = (ClampMin = "0.0"))
    float MergeDistanceFactor = 2000.f; // 合并距离因子
    
    UPROPERTY(EditAnywhere, Category = "QuadTree")
    bool bDrawDebug = true; // 是否绘制调试信息
    
    UPROPERTY(EditAnywhere, Category = "QuadTree")
    FColor DebugColor = FColor::Green; // 调试颜色
    
    UPROPERTY(EditAnywhere, Category = "QuadTree")
    TSubclassOf<class UUserWidget> WidgetClass; // 要生成的Widget类

    UPROPERTY(EditAnywhere, Category = "QuadTree")
    FVector2D WidgetSize = FVector2D(16.f, 16.f);

    UPROPERTY(EditAnywhere, Category= "QuadTree")
    float WidgetHeight = 100.0f;
};