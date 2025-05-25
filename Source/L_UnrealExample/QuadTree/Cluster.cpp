// Fill out your copyright notice in the Description page of Project Settings.


#include "L_UnrealExample/QuadTree/Cluster.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ACluster::ACluster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetConstraintMode(EDOFMode::XYPlane);
	GetStaticMeshComponent()->SetSimulatePhysics(true);	
}

// Called when the game starts or when spawned
void ACluster::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("Clusters Start!"));
}

// Called every frame
void ACluster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Display, TEXT("Clusters Tick!"));
	if (bActive && targetActor)
	{
		float drawTime = 1 / UKismetSystemLibrary::GetFrameCount();
		DrawDebugLine(GetWorld(), GetActorLocation(), targetActor->GetActorLocation(),
			FColor(0,148,220,255), false, drawTime, 1, 4.0f);
	}
}

void ACluster::ActiveState(bool _bActive, AActor* _targetActor)
{
	if (bActive == _bActive)
		return;
	bActive = _bActive;
	targetActor = _targetActor;
	GetStaticMeshComponent()->SetMaterial(0, bActive ? m_active : m_normal);
}

