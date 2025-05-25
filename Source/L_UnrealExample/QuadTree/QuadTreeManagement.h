// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuadTreeManagement.generated.h"

UCLASS()
class L_UNREALEXAMPLE_API AQuadTreeManagement : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AQuadTreeManagement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	TSubclassOf<ACluster> ClusterClass;

	UPROPERTY(EditAnywhere)
	AActor* traceActor;

	UPROPERTY(EditAnywhere)
	float affectRadianRange=50;

	UPROPERTY()
	TArray<ACluster*> objs;

	TSharedPtr<AQuadTreeManagement> root;
	
	FTimerHandle Timer_SpawnActors;
	FTimerHandle Timer_AddActorsVelocity;

public:
	void SpawnActors();
	void ActorsAddVelocity();

	UFUNCTION(BlueprintCallable)
	void GetLeafPoint();

	UFUNCTION(BlueprintCallable)
	void GetTraverse();

	UFUNCTION(BlueprintCallable)
	void GetTreeDepth(int32 depth);
};
