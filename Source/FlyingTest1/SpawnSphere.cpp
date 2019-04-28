// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnSphere.h"

// Sets default values
ASpawnSphere::ASpawnSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereMesh = CreateDefaultSubobject<UStaticMeshComponent>("Sphere");

	sphereMat = CreateDefaultSubobject<UMaterial>(TEXT("Sphere Material"));
}

// Called when the game starts or when spawned
void ASpawnSphere::BeginPlay()
{
	Super::BeginPlay();
	sphereMesh->SetMaterial(0, sphereMat);
}

// Called every frame
void ASpawnSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

