// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battery.h"
#include "UObject/Object.h"

/**
 * 
 */
class L_UNREALEXAMPLE_API QuadTreeNode : public TSharedFromThis<QuadTreeNode>
{
public:
	FVector center; // 中心点
	FVector extend; // 扩展尺寸
	bool isLeaf;    //是否是叶子节点
	int32 depth = 0;
	int32 maxCount = 4;

	TArray<ABattery*>objs; 
	static UObject* worldObject;
	bool bInRange;
	
	TSharedPtr<QuadTreeNode> root;
	TArray<TSharedPtr<QuadTreeNode>> child_node;
	
public:
	QuadTreeNode(FVector _center, FVector _extend, int32 _depth, TSharedPtr<QuadTreeNode> _root=nullptr);

	~QuadTreeNode();

	inline bool IsNotUsed()
	{
		return isLeaf && objs.Num() <= 0;
	}

	//方形与圆形求交
	bool InterSection(FVector _OCenter, float _radian);

	//点是否在本区域内
	bool InterSection(FVector _point);

	//点是否在指定区域内
	bool InterSection(FVector _pMin, FVector _pMax, FVector _point);
	
	//插入对象
	void InsertObj(ABattery* obj);

	// 绘制区域边界
	void DrawBound(float time = 0.02f, float thickness = 2.0f);

	// 判断电池是否在扫描器的范围类
	void TraceObjectInRange(AActor* traceActor, float _radian);	

	void TraceObjectOutRange(FVector _OCenter, float _radian);
	// 更新状态
	void UpdateState();	
};
