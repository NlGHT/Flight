// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnSphere.generated.h"

UCLASS()
class FLYINGTEST1_API ASpawnSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnSphere();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* sphereMesh;

	UPROPERTY(EditAnywhere)
		class UMaterialInterface* sphereMat;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
