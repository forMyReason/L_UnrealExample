// Fill out your copyright notice in the Description page of Project Settings.


#include "L_UnrealExample/Public/QuadTree/QuadTree.h"
#include "Kismet/KismetMathLibrary.h"
#include "L_UnrealExample/Private/QuadTree/QuadTreeManager.cpp"


AQuadTree::AQuadTree()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

UObject* QuadTreeManager::worldObject = nullptr;

// Called when the game starts
void AQuadTree::BeginPlay()
{
	Super::BeginPlay();

	QuadTreeManager::worldObject = GetWorld();
	root = MakeShareable(new QuadTreeManager(FVector::ZeroVector, FVector(height, width, 0), 0));
	GetWorld()->GetTimerManager().SetTimer(Timer_SpawnActors, this, &AQuadTree::SpawnActors, playRate, true);
	GetWorld()->GetTimerManager().SetTimer(Timer_AddActorsVelocity, this, &AQuadTree::ActorsAddVelocity, 2, true);
}

// Called every frame
void AQuadTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (root.IsValid())
	{
		root->UpdateState(); //更新状态
		root->TraceObjectInRange(traceActor, affectRadianRange); //判断是否在扫描器的范围内	
	}
}

// 定时生成物体
void AQuadTree::SpawnActors()
{
	if (cubeCount < 0) {
		GetWorld()->GetTimerManager().ClearTimer(Timer_SpawnActors);
		return;
	}
	cubeCount--;
	FTransform trans = FTransform(
		FRotator(0, UKismetMathLibrary::RandomFloatInRange(0, 360), 0),
		FVector(UKismetMathLibrary::RandomIntegerInRange(-height+10, height-10),
		UKismetMathLibrary::RandomIntegerInRange(-width+10, width-10), 11), FVector(0.2));
	ABattery* actor= GetWorld()->SpawnActor<ABattery>(BatteryClass, trans);
	if (IsValid(actor))
	{
		objs.Add(actor);
		root->InsertObj(actor);	
	}
}

// 定时给物体一个速度
void AQuadTree::ActorsAddVelocity()
{
	for (ABattery* actor :objs)
	{
		actor->GetStaticMeshComponent()->SetPhysicsLinearVelocity(UKismetMathLibrary::RandomUnitVector() * 50);
	}
}



