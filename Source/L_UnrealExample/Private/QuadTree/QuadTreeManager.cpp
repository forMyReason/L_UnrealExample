// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "L_UnrealExample/QuadTree/Cluster.h"

class QuadTreeManager : public TSharedFromThis<QuadTreeManager> {

public:
	FVector center; // 中心点
	FVector extend; // 扩展尺寸
	bool isLeaf;    //是否是叶子节点
	int32 depth = 0;
	int32 maxCount = 4;

	TArray<ABattery*> objs; 
	static UObject* worldObject;
	bool bInRange;
	
	TSharedPtr<QuadTreeManager> root;
	TArray<TSharedPtr<QuadTreeManager>> child_node;
public:
	QuadTreeManager(FVector _center, FVector _extend, int32 _depth, TSharedPtr<QuadTreeManager> _root=nullptr)
		: center(_center), extend(_extend), depth(_depth)
	{
		root = _root;
		isLeaf = true;
		child_node.Init(nullptr, 4);
		bInRange = false;
	}
	
	~QuadTreeManager()
	{
		root = nullptr;
		objs.Empty();
		child_node.Empty();
	}

	bool IsNotUsed() const
	{
		return isLeaf && objs.Num() <= 0;
	}
	
	/// @param _OCenter 圆心位置
	/// @param _radian 半径
	/// @return 球体和矩形是否相交
	bool InterSection(FVector _OCenter, float _radian) const
	{
		FVector v = _OCenter - center; //取相对原点
		float x = UKismetMathLibrary::Min(v.X, extend.X); 
		x = UKismetMathLibrary::Max(x, -extend.X);

		float y = UKismetMathLibrary::Min(v.Y, extend.Y);
		y = UKismetMathLibrary::Max(y, -extend.Y);
		
		return (x - v.X) * (x - v.X) + (y - v.Y) * (y - v.Y) <= _radian * _radian; //注意此时圆心的相对坐标
	}

	//点是否在本区域内
	bool InterSection(FVector _point) const
	{	
		return (
			_point.X >= center.X - extend.X &&
			_point.X <= center.X + extend.X &&
			_point.Y >= center.Y - extend.Y &&
			_point.Y <= center.Y + extend.Y
		);
	}

	//点是否在指定区域内
	bool InterSection(FVector _pMin, FVector _pMax, FVector _point)
	{		
		return (
			_point.X >= _pMin.X &&
			_point.X <= _pMax.X &&
			_point.Y >= _pMin.Y &&
			_point.Y <= _pMax.Y
		);
	}
	
	//插入对象
	void InsertObj(ABattery* obj)
	{
		objs.Add(obj);
		if (isLeaf && objs.Num() <= maxCount) //直接插入			
		{
			return;
		}	

		float dx[4] = { 1, -1, -1, 1 };
		float dy[4] = { 1, 1, -1, -1 };
		//超过上限个数，创建子节点;或者不再是叶子节点
		isLeaf = false;
		for (auto& item : objs) {
			for (int i = 0; i < 4; i++)
			{
				//四个象限
				FVector p = center + FVector(extend.X * dx[i], extend.Y * dy[i], 0);
				FVector pMin = p.ComponentMin(center);
				FVector pMax = p.ComponentMax(center);
				if (InterSection(pMin, pMax, item->GetActorLocation())) {
					if (!child_node[i].IsValid())
					{
						root = root.IsValid() ? root : this->AsShared();
						child_node[i] = MakeShareable(new QuadTreeManager(pMin/2+pMax/2, extend / 2, depth + 1, root));
					}
					child_node[i]->InsertObj(item);
					//break; //确保只在一个象限内
				}
			}
		}
		objs.Empty(); //确保非叶子节点不存
	}

	// 绘制区域边界
	void DrawBound(float time = 0.02f, float thickness = 2.0f) {	
		if (worldObject)
		{
			FLinearColor drawColor = bInRange ? FLinearColor::Green : FLinearColor::Red;
			FVector drawCenter = center + (bInRange ? FVector(0, 0, 8) : FVector(0, 0, 5));
			UKismetSystemLibrary::DrawDebugBox(worldObject, drawCenter, extend+FVector(0,0,1), drawColor, FRotator::ZeroRotator, time, thickness);
		}
	}

	// 判断电池是否在扫描器的范围类
	void TraceObjectInRange(AActor* traceActor, float _radian)
	{
		FVector _OCenter = traceActor->GetActorLocation();
		if (!InterSection(_OCenter, _radian))
			TraceObjectOutRange(_OCenter, _radian);
		else
		{
			bInRange = true;
			if (isLeaf)
			{
				for (ABattery* obj : objs)
				{					
					_OCenter.Z = obj->GetActorLocation().Z;
					bool bCanActive = FVector::Distance(_OCenter, obj->GetActorLocation()) <= _radian;
					obj->ActiveState(bCanActive, traceActor);
				}
			}
			else
			{
				for (auto& node : child_node)
				{
					if (node.IsValid()) {
						node->TraceObjectInRange(traceActor, _radian);
					}
				}
			}
		}
	}
	

	void TraceObjectOutRange(FVector _OCenter, float _radian) {
		bInRange = false;
		for (ABattery* obj : objs){
			{				
				obj->ActiveState(false, nullptr);
			}
		}
		for (auto& node: child_node)
		{

			if (node.IsValid()) {
				node->TraceObjectOutRange(_OCenter, _radian);
			}			
		}
	}

	// 更新状态
	void UpdateState() {
		DrawBound(1 / UKismetSystemLibrary::GetFrameCount()); //根据帧数绘制

		if (!isLeaf) {	//如果不是叶子节点，则递归到子树下去，如果子树为空，则回收该节点
			for (auto& node : child_node){
				if (node.IsValid())
				{
					node->UpdateState();
					if (node->IsNotUsed())
					{
						node.Reset();
						node = nullptr;	
					}					
				}
			}

			int32 count = 4;
			for (auto& node : child_node) {
				if (!node.IsValid())
					count--;
			}
			if (count == 0) {
				isLeaf = true;
			}else
				return;
		}
		
		if (isLeaf && objs.Num()>0){ //如果叶子节点，更新物体是否在区域内；不在区域则移出，并重新插入
			int32 i = 0;
			while (i<objs.Num())
			{
				if (!InterSection(objs[i]->GetActorLocation())) {	
					ABattery* battery = objs[i];
					objs.Swap(i, objs.Num() - 1);
					objs.Pop();
					root->InsertObj(battery);
					continue;
				}
				i++;
			}
		}
	}
};
