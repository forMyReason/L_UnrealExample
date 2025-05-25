#include "QuadTreeNode.h"
#include "Kismet/KismetSystemLibrary.h"

void QuadTreeNode::InsertObj(ACluster* obj) {
	NodeNumber++;
	objs.Add(obj);
	if (isLeaf && objs.Num() <= maxCount) {
		return;
	}
	float dx[4] = { 1, -1,-1, 1 };
	float dy[4] = { 1, 1, -1, -1 };
	isLeaf = false;
	for (auto& item : objs) {
		for (int i = 0; i < 4; i++) {
			FVector p = center + FVector(extend.X * dx[i], extend.Y * dy[i], 0);
			FVector pMin = p.ComponentMin(center);
			FVector pMax = p.ComponentMax(center);
			if (InterSection(pMin, pMax, item->GetActorLocation())) {
				if (!child_node[i].IsValid()) {
					root = root.IsValid() ? root : this->AsShared();
					child_node[i] = MakeShareable(new QuadTreeNode(pMin / 2 + pMax / 2, extend / 2, depth + 1, World, root));
				}
				child_node[i]->InsertObj(item);
				child_node[i]->DrawBound();
				objs.Empty();
			}
		}
	}
}
bool QuadTreeNode::InterSection(FVector _pMin, FVector _pMax, FVector _point) {
	return (
		_point.X >= _pMin.X &&
		_point.X <= _pMax.X &&
		_point.Y >= _pMin.Y &&
		_point.Y <= _pMax.Y
	);
}
void QuadTreeNode::DrawBound() {
	if (World) {
		FLinearColor drawColor = bInRange ? FLinearColor::Green : FLinearColor::Red;
		FVector drawCenter = center + (bInRange ? FVector(0, 0, 8) : FVector(0, 0, 5));
		UKismetSystemLibrary::DrawDebugBox(World, drawCenter, extend + FVector(0, 0, 1), drawColor, FRotator::ZeroRotator, 1000, 10);
	}
}
void QuadTreeNode::GetTreeDepth(int32 Depth) {
	if (!isLeaf) {
		if (depth == Depth) {
			CreateNumWidget(ObjCenterLocation, NodeNumber,true);
		} else {
			CreateNumWidget(ObjCenterLocation, NodeNumber, false);
		}
		for (auto& node : child_node) {
			if (node.IsValid()) {
				node->GetTreeDepth(Depth);
			}
		}
	} else {
		if (depth <= Depth) {
			for (auto obj : objs) {
				obj->SetNodeState(true);
			}
		} else {
			for (auto obj : objs) {
				obj->SetNodeState(false);
			}
		}
	}
}
void QuadTreeNode::GetNodeNumber()
{
	
}

void QuadTreeNode::RefreshObjLocation() {
	if (!isLeaf) {
		for (auto& node : child_node) {
			if (node.IsValid()) {
				node->RefreshObjLocation();
			}
		}
	} else {
		TArray<FVector2D> LocationTArray;
		for (auto Obj:objs) {
			LocationTArray.Add(FVector2D(Obj->GetActorLocation().X, Obj->GetActorLocation().Y));
		}
		ObjCenterLocation = FVector(UBFL_3DAnalysis::GetPolygonCenter(LocationTArray),center.Z);
	}
}

void QuadTreeNode::Traverse() {
	if (!isLeaf) {
		TArray<FVector2D> NodeLocationTArray;
		for (auto& node : child_node) {
			if (node.IsValid()) {
				node->Traverse();
				NodeLocationTArray.Add(FVector2D(node->ObjCenterLocation.X,node->ObjCenterLocation.Y));
			}
		}
		ObjCenterLocation = FVector(UBFL_3DAnalysis::GetPolygonCenter(NodeLocationTArray), center.Z);
	} else {
		RefreshObjLocation();
	}
}

void QuadTreeNode::CreateNumWidget(FVector widgetLocation, int32 Number,bool IsShowNumWidget)
{
	if (IsShowNumWidget) {
		UClass* WidgetClass = StaticLoadClass(ACluster::StaticClass(), nullptr, TEXT("Blueprint'/FrameWork/Tools/cluster/BP_Cluster.BP_Cluster_C'"));
		if (WidgetClass) {
			FTransform Tran = FTransform(FRotator(0, 0, 0), widgetLocation, FVector(1, 1, 1));
			if (UIActor == nullptr) {
				UIActor = World->SpawnActor<ACluster>(WidgetClass, Tran);
			}
			if (UIActor)
			{
				UIActor->SetNodeState(true);
				UIActor->InitWidget(Number);
			}
		}
	} else {
		if (UIActor) {
			UIActor->SetNodeState(false);
		}
	}
}