// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapGenerate.generated.h"

UCLASS()
class FLYINGTEST1_API AMapGenerate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapGenerate();

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ASpawnRod> spawnRodBP;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ASpawnSphere> spawnSphereBP;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* groundTerrain;

	UPROPERTY(EditAnywhere)
		int spheresToSpawn = 300;

	UPROPERTY(EditAnywhere)
		int rodsToSpawn = 75;

	UPROPERTY(EditAnywhere)
		float zDisplacement = 3.f;

	FVector groundSize;



	// Functions
	UFUNCTION(BlueprintCallable)
		void spawnSphere(FVector scaledBounds, FVector rootPosition);

	UFUNCTION(BlueprintCallable)
		void spawnRod(FVector scaledBounds, FVector rootPosition, int i);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void printToScreenDebug(FTransform transform);
	void printToScreenDebug(FVector vector);
	void printToScreenDebug(float floatVal);

	FVector getScaledBounds(UStaticMeshComponent* meshComponent);

	TArray<ASpawnSphere*> spheres;
	TArray<ASpawnRod*> rods;
};
