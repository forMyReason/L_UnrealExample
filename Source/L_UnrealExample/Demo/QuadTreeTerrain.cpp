#include "QuadTreeTerrain.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "QuadTree/QuadTreeNode.h"

AQuadTreeTerrain::AQuadTreeTerrain()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AQuadTreeTerrain::BeginPlay()
{
    Super::BeginPlay();     // 调用该父类的BeginPlay函数
    BuildQuadTree();
}

void AQuadTreeTerrain::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (RootNode.IsValid())
    {
        const FVector CameraLocation = GetCameraLocation();
        SubdivideNode(RootNode, CameraLocation);
        
        if (bDrawDebug)
        {
            DrawDebugQuadTree(RootNode);
        }
    }
}

void AQuadTreeTerrain::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ClearQuadTree();
    BuildQuadTree();
}

void AQuadTreeTerrain::BuildQuadTree()
{
    // 创建根节点
    RootNode = MakeShared<QuadTreeNode>(
        FVector2D(GetActorLocation().X, GetActorLocation().Y), 
        TerrainSize, 
        0
    );
    
    // 创建根节点Widget
    CreateOrUpdateWidget(RootNode);
}

void AQuadTreeTerrain::SubdivideNode(TSharedPtr<QuadTreeNode> Node, const FVector& CameraLocation)
{
    if (!Node.IsValid()) return;
    
    // 检查是否需要细分
    if (!Node->IsSubdivided && ShouldSubdivide(*Node, CameraLocation) && Node->Depth < MaxDepth)
    {
        // 计算子节点尺寸
        const FVector2D ChildSize = Node->Size * 0.5f;
        const FVector2D HalfChildSize = ChildSize * 0.5f;
        
        // 创建四个子节点
        // 西北 (North-West)
        Node->NW = MakeShared<QuadTreeNode>(
            FVector2D(Node->Center.X - HalfChildSize.X, Node->Center.Y + HalfChildSize.Y),
            ChildSize,
            Node->Depth + 1
        );
        
        // 东北 (North-East)
        Node->NE = MakeShared<QuadTreeNode>(
            FVector2D(Node->Center.X + HalfChildSize.X, Node->Center.Y + HalfChildSize.Y),
            ChildSize,
            Node->Depth + 1
        );
        
        // 西南 (South-West)
        Node->SW = MakeShared<QuadTreeNode>(
            FVector2D(Node->Center.X - HalfChildSize.X, Node->Center.Y - HalfChildSize.Y),
            ChildSize,
            Node->Depth + 1
        );
        
        // 东南 (South-East)
        Node->SE = MakeShared<QuadTreeNode>(
            FVector2D(Node->Center.X + HalfChildSize.X, Node->Center.Y - HalfChildSize.Y),
            ChildSize,
            Node->Depth + 1
        );
        
        // 标记节点已细分
        Node->IsSubdivided = true;
        
        // 移除父节点Widget
        if (Node->WidgetComponent)
        {
            Node->WidgetComponent->DestroyComponent();
            Node->WidgetComponent = nullptr;
        }
        
        // 为子节点创建Widget
        CreateOrUpdateWidget(Node->NW);
        CreateOrUpdateWidget(Node->NE);
        CreateOrUpdateWidget(Node->SW);
        CreateOrUpdateWidget(Node->SE);
    }
    
    // 递归处理子节点
    if (Node->IsSubdivided)
    {
        SubdivideNode(Node->NW, CameraLocation);
        SubdivideNode(Node->NE, CameraLocation);
        SubdivideNode(Node->SW, CameraLocation);
        SubdivideNode(Node->SE, CameraLocation);
        
        // 检查是否所有子节点都应该合并
        bool bAllChildrenShouldMerge = true;
        bAllChildrenShouldMerge &= ShouldMerge(*Node->NW, CameraLocation);
        bAllChildrenShouldMerge &= ShouldMerge(*Node->NE, CameraLocation);
        bAllChildrenShouldMerge &= ShouldMerge(*Node->SW, CameraLocation);
        bAllChildrenShouldMerge &= ShouldMerge(*Node->SE, CameraLocation);
        
        if (bAllChildrenShouldMerge)
        {
            MergeNodes(Node, CameraLocation);
        }
    }
}

void AQuadTreeTerrain::MergeNodes(TSharedPtr<QuadTreeNode> Node, const FVector& CameraLocation)
{
    if (!Node.IsValid() || !Node->IsSubdivided) return;
    
    // 递归合并子节点的子节点
    MergeNodes(Node->NW, CameraLocation);
    MergeNodes(Node->NE, CameraLocation);
    MergeNodes(Node->SW, CameraLocation);
    MergeNodes(Node->SE, CameraLocation);
    
    // 删除所有子节点及其Widget
    RemoveNodeAndWidget(Node->NW);
    RemoveNodeAndWidget(Node->NE);
    RemoveNodeAndWidget(Node->SW);
    RemoveNodeAndWidget(Node->SE);
    
    // 重置子节点指针
    Node->NW.Reset();
    Node->NE.Reset();
    Node->SW.Reset();
    Node->SE.Reset();
    
    // 标记节点未细分
    Node->IsSubdivided = false;
    
    // 为当前节点创建Widget
    CreateOrUpdateWidget(Node);
}

void AQuadTreeTerrain::CreateOrUpdateWidget(TSharedPtr<QuadTreeNode> Node)
{
    if (!Node.IsValid() || !WidgetClass) return;
    
    // 如果Widget不存在，创建新的
    if (!Node->WidgetComponent)
    {
        Node->WidgetComponent = NewObject<UWidgetComponent>(this);
        Node->WidgetComponent->RegisterComponent();
        Node->WidgetComponent->SetWidgetClass(WidgetClass);
        Node->WidgetComponent->SetDrawSize(WidgetSize);
        Node->WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        
        // 设置Widget位置为节点中心
        const FVector WidgetLocation(Node->Center.X, Node->Center.Y, WidgetHeight);
        Node->WidgetComponent->SetWorldLocation(WidgetLocation);
        
        // 设置Widget始终面向摄像机
        Node->WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    }
    
    // 更新Widget缩放比例（基于深度）
    const float Scale = 1.0f / FMath::Pow(2.0f, Node->Depth);
    Node->WidgetComponent->SetWorldScale3D(FVector(Scale));
    
    // 如果Widget已存在，直接更新内容
    if (UUserWidget* UserWidget = Cast<UUserWidget>(Node->WidgetComponent->GetUserWidgetObject()))
    {
        if (UTextBlock* TextBlock = Cast<UTextBlock>(UserWidget->GetWidgetFromName(TEXT("TextBlock_120"))))
        {
            TextBlock->SetText(FText::FromString(FString::FromInt(Node->Depth)));
        }
    }
}

void AQuadTreeTerrain::RemoveNodeAndWidget(TSharedPtr<QuadTreeNode> Node)
{
    if (!Node.IsValid()) return;
    
    // 递归删除子节点
    if (Node->IsSubdivided)
    {
        RemoveNodeAndWidget(Node->NW);
        RemoveNodeAndWidget(Node->NE);
        RemoveNodeAndWidget(Node->SW);
        RemoveNodeAndWidget(Node->SE);
    }
    
    // 删除Widget组件
    if (Node->WidgetComponent)
    {
        Node->WidgetComponent->DestroyComponent();
        Node->WidgetComponent = nullptr;
    }
}

void AQuadTreeTerrain::DrawDebugQuadTree(TSharedPtr<QuadTreeNode> Node)
{
    if (!Node.IsValid() || !GetWorld()) return;
    
    // 绘制当前节点边界
    const FVector Center3D(Node->Center.X, Node->Center.Y, 0.f);
    const FVector Extent(Node->Size.X * 0.5f, Node->Size.Y * 0.5f, 10.f);
    
    // 根据深度调整颜色
    const float DepthRatio = static_cast<float>(Node->Depth) / MaxDepth;
    const FColor Color = FColor::MakeRedToGreenColorFromScalar(1.0f - DepthRatio);
    
    DrawDebugBox(GetWorld(), Center3D, Extent, FQuat::Identity, Color, false, -1.f, 0, 2.f);
    
    // 递归绘制子节点
    if (Node->IsSubdivided)
    {
        DrawDebugQuadTree(Node->NW);
        DrawDebugQuadTree(Node->NE);
        DrawDebugQuadTree(Node->SW);
        DrawDebugQuadTree(Node->SE);
    }
}

FVector AQuadTreeTerrain::GetCameraLocation() const
{
    APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
    if (CameraManager)
    {
        return CameraManager->GetCameraLocation();
    }
    return FVector::ZeroVector;
}

float AQuadTreeTerrain::GetDistanceToCamera(const FVector2D& NodeCenter) const
{
    const FVector CameraLocation = GetCameraLocation();
    // 四叉树主要用于分割平面，z坐标没有意义
    const FVector NodeCenter3D(NodeCenter.X, NodeCenter.Y, 0.f);
    return FVector::Dist(CameraLocation, NodeCenter3D);
}

bool AQuadTreeTerrain::ShouldSubdivide(const QuadTreeNode& Node, const FVector& CameraLocation) const
{
    const float Distance = GetDistanceToCamera(Node.Center);
    return Distance < (SubdivideDistanceFactor / FMath::Pow(2.0f, Node.Depth));
}

bool AQuadTreeTerrain::ShouldMerge(const QuadTreeNode& Node, const FVector& CameraLocation) const
{
    const float Distance = GetDistanceToCamera(Node.Center);
    return Distance > (MergeDistanceFactor / FMath::Pow(2.0f, Node.Depth - 1));
}

void AQuadTreeTerrain::ClearQuadTree()
{
    if (RootNode.IsValid())
    {
        RemoveNodeAndWidget(RootNode);
        RootNode.Reset();
    }
}