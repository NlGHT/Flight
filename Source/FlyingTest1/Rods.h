// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rods.generated.h"

UCLASS()
class FLYINGTEST1_API ARods : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARods();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
