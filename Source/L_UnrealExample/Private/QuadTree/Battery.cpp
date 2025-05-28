// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTree/Battery.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABattery::ABattery()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetConstraintMode(EDOFMode::XYPlane);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void ABattery::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABattery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bActive && targetActor)
	{
		float drawTime = 1 / UKismetSystemLibrary::GetFrameCount();
		DrawDebugLine(GetWorld(), GetActorLocation(), targetActor->GetActorLocation(),
			FColor(0,148,220,255), false, drawTime, 1, 4.0f);
	}
}

void ABattery::ActiveState(bool _bActive, AActor* _targetActor){
	if (bActive == _bActive)
		return;
	bActive = _bActive;
	targetActor = _targetActor;
	GetStaticMeshComponent()->SetMaterial(0, bActive ? m_active : m_normal);
}
