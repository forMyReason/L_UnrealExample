#pragma once
#include "Cluster.h"

class FRAMEWORK_API QuadTreeNode :public TSharedFromThis<QuadTreeNode>
{
public:
	QuadTreeNode(FVector _center, FVector _extend, int32 _depth, UWorld* _world, TSharedPtr<QuadTreeNode> _root = nullptr) :center(_center), extend(_extend), depth(_depth) {
		root = _root;
		isLeaf = true;
		child_node.Init(nullptr, 4);
		bInRange = false;
		World = _world;
	};
	~QuadTreeNode() {
		root = nullptr;
		objs.Empty();
		child_node.Empty();
	};

public:
	FVector center; // 中心点
	FVector extend; // 扩展尺寸
	bool isLeaf;  //是否是叶子节点
	int32 depth = 0;
	int32 maxCount = 4;
	
	//其中包括的类
	TArray<ACluster*>objs;

	FVector ObjCenterLocation;
	int32 NodeNumber = 0;

	//static UObject* worldObject;
	UWorld* World;
	bool bInRange;
	ACluster* UIActor = nullptr;
	TSharedPtr<QuadTreeNode> root;
	TArray<TSharedPtr<QuadTreeNode>> child_node;

	//插入对象
	void InsertObj(ACluster* obj);
	bool InterSection(FVector _pMin, FVector _pMax, FVector _point);
	void DrawBound();

	//输出深度
	void GetTreeDepth(int32 Depth);
	//获取节点下的所有数量
	void GetNodeNumber();

	void RefreshObjLocation();

	//遍历四叉树
	void Traverse();
	//生成actor聚类

	void CreateNumWidget(FVector widgetLocation, int32 Number, bool IsShowNumWidget);
};
