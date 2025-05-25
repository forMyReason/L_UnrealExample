// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "./Cluster.h"
#include "QuadTree.generated.h"
inline TSharedPtr<class QuadTreeManager> root;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class L_UNREALEXAMPLE_API AQuadTree : public AActor
{
	GENERATED_BODY()

public:
	AQuadTree();

protected:
	// Called when the game starts
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

	TSharedPtr<QuadTreeManager> root;
	
	FTimerHandle Timer_SpawnActors;
	FTimerHandle Timer_AddActorsVelocity;
};

