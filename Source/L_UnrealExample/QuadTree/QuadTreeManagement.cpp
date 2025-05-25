// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTreeManagement.h"
#include "Cluster.h"
#include "QuadTreeNode.h"
#include "Kismet/KismetMathLibrary.h"
#include "L_UnrealExample/QuadTree/Cluster.h"


// Sets default values
AQuadTreeManagement::AQuadTreeManagement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void AQuadTreeManagement::BeginPlay()
{
	Super::BeginPlay();
	root = MakeShareable(new QuadTreeNode(
		FVector::ZeroVector,FVector(height,width,0),0,GetWorld()
	));

	int32 max = cubeCount;
	for (int32 i = 0; i < max; i++)
	{
		SpawnActors();
	}

	GetTraverse();
}

// Called every frame
void AQuadTreeManagement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuadTreeManagement::SpawnActors()
{
	if (cubeCount < 0)
	{
		return;
	}
	FVector pos = FVector(UKismetMathLibrary::RandomIntegerInRange(-height + 10, height - 10),
		UKismetMathLibrary::RandomIntegerInRange(-width + 10, width - 10), 11);
	FTransform trans = FTransform(FRotator(0, UKismetMathLibrary::RandomFloatInRange(0, 360), 0), pos, FVector(0.2));
	ACluster* actor = GetWorld()->SpawnActor<ACluster>(ClusterClass, trans);

	if (IsValid(actor))
	{
		objs.Add(actor);
		root->InsertObj(actor);
	}
}

void AQuadTreeManagement::ActorsAddVelocity()
{
	
}

void AQuadTreeManagement::GetLeafPoint()
{
	for (auto obj : root->objs)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Name:%s"), *obj->GetActorNameOrLabel());
	}
}

void AQuadTreeManagement::GetTraverse()
{
	root.Get()->Traverse();
}

void AQuadTreeManagement::GetTreeDepth(int32 depth)
{
	root.Get()->GetTreeDepth(depth);
}

