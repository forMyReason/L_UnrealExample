// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Battery.h"
#include "QuadTreeNode.h"
#include "QuadTree.generated.h"

UCLASS()
class L_UNREALEXAMPLE_API AQuadTree : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AQuadTree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnActors();
	void ActorsAddVelocity();
	
public:
	UPROPERTY(EditAnywhere)
	int32 cubeCount=20;
	
	UPROPERTY(EditAnywhere)
	int32 width=500;

	UPROPERTY(EditAnywhere)
	int32 height=500;

	UPROPERTY(EditAnywhere)
	float playRate=0.05;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABattery> BatteryClass;

	UPROPERTY(EditAnywhere)
	AActor* traceActor;

	UPROPERTY(EditAnywhere)
	float affectRadianRange=50;

	UPROPERTY()
	TArray<ABattery*> objs;

	TSharedPtr<QuadTreeNode> root;
	FTimerHandle timer;
	FTimerHandle timer2;
};
