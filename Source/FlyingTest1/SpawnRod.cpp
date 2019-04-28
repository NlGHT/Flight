// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnRod.h"

// Sets default values
ASpawnRod::ASpawnRod()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rodMesh = CreateDefaultSubobject<UStaticMeshComponent>("Rod");

	material = CreateDefaultSubobject<UMaterial>(TEXT("Rod Material"));
}

// Called when the game starts or when spawned
void ASpawnRod::BeginPlay()
{
	Super::BeginPlay();
	rodMesh->SetMaterial(0, material);
}

// Called every frame
void ASpawnRod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

