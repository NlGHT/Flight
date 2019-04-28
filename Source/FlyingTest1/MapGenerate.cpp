// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerate.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine.h"
#include "SpawnRod.h"
#include "SpawnSphere.h"

// Sets default values
AMapGenerate::AMapGenerate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	groundTerrain = CreateDefaultSubobject<UStaticMeshComponent>("Ground");

}

// Called when the game starts or when spawned
void AMapGenerate::BeginPlay()
{
	Super::BeginPlay();

	FVector scaledBounds = getScaledBounds(groundTerrain);
	FVector position = groundTerrain->GetComponentTransform().GetLocation();
	//printToScreenDebug(scaledBounds);

	for (int i = 0; i < spheresToSpawn; i++) {
		spawnSphere(scaledBounds, position);
	}

	for (int i = 0; i < rodsToSpawn; i++) {
		spawnRod(scaledBounds, position, i);
	}

	
}

// Called every frame
void AMapGenerate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapGenerate::printToScreenDebug(FTransform transform)
{
	FVector transformVector;
	transform.TransformVector(transformVector);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Ground transform vector: X > %f, Y > %f, Z > %f"), transformVector.X, transformVector.Y, transformVector.Z));
}

void AMapGenerate::printToScreenDebug(FVector vector)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Vector: X > %f, Y > %f, Z > %f"), vector.X, vector.Y, vector.Z));
}

void AMapGenerate::printToScreenDebug(float floatVal)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("float: %f"), floatVal));
}

FVector AMapGenerate::getScaledBounds(UStaticMeshComponent* meshComponent)
{
	FVector groundMin;
	FVector groundMax;

	meshComponent->GetLocalBounds(groundMin, groundMax);
	FVector dimensions = FVector(groundMax.X - groundMin.X, groundMax.Y - groundMin.Y, groundMax.Z - groundMin.Z);

	FVector scale = meshComponent->GetComponentTransform().GetScale3D();

	//printToScreenDebug(transformVector);
	//printToScreenDebug(scale);

	FVector scaledSize = FVector(dimensions.X * scale.X, dimensions.Y * scale.Y, dimensions.Z * scale.Z);


	return scaledSize;
}

void AMapGenerate::spawnSphere(FVector scaledBounds, FVector rootPosition)
{
	FVector halfDimensions = scaledBounds/2;
	float rootPositionZ = -rootPosition.Z;
	float scaledBoundsZ = -scaledBounds.Z;
	float differenceOfRootAndScaledBounds = (rootPositionZ + scaledBoundsZ);
	//printToScreenDebug(differenceOfRootAndScaledBounds);
	//printToScreenDebug((rootPosition.Z + scaledBounds.Z));
	float RNG = ((float)FMath::Rand()) / 32767 * zDisplacement * (rootPosition.Z + scaledBounds.Z) - 20*scaledBounds.Z - zDisplacement * scaledBounds.Z;
	printToScreenDebug(RNG);

	FVector randomSpawnLocation = FVector(FMath::RandRange(rootPosition.X - halfDimensions.X, rootPosition.X + halfDimensions.X), FMath::RandRange(rootPosition.Y - halfDimensions.Y, rootPosition.Y + halfDimensions.Y), RNG);

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;

	FRotator rotator;

	UWorld* world = GetWorld();
	if (world)
	{
		ASpawnSphere* spawnedSphere = world->SpawnActor<ASpawnSphere>(spawnSphereBP, randomSpawnLocation, rotator, spawnParams);
		spheres.Add(spawnedSphere);

		int maxRand = 60;
		
		UStaticMeshComponent* sphereMesh = Cast<UStaticMeshComponent>(spawnedSphere->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		sphereMesh->SetRelativeScale3D(FVector(FMath::RandRange(5, maxRand)));

		float radiusNewlySpawned = getScaledBounds(sphereMesh).X / 2;
		for (int i = 0; i < spheres.Num(); i++)
		{
			ASpawnSphere* listSphere = spheres[i];
			UStaticMeshComponent* sphereMeshOfListItem = Cast<UStaticMeshComponent>(spheres[i]->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			float radiusOfListItem = getScaledBounds(sphereMeshOfListItem).X / 2;
			//printToScreenDebug(radiusOfListItem);

			if (FVector::Dist(sphereMeshOfListItem->GetComponentTransform().GetLocation(), randomSpawnLocation) < radiusNewlySpawned*2 + radiusOfListItem*2)
			{
				// If the spheres are overlapping
				spheres.RemoveAt(i);
				Destroy(listSphere);
			}

		}

	}
	//GetWorld()->SpawnActor()

	//printToScreenDebug(halfDimensions);
}

void AMapGenerate::spawnRod(FVector scaledBounds, FVector rootPosition, int i)
{
	float yPosition = (rootPosition.Y - scaledBounds.Y/2) + ((i + 1) * (scaledBounds.Y / rodsToSpawn));
	//printToScreenDebug(yPosition);
	FVector spawnLocation = FVector(yPosition, rootPosition.Y - scaledBounds.Y/2, rootPosition.Z);

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;

	FRotator rotator = FRotator(0, 0, 0);

	UWorld* world = GetWorld();
	if (world)
	{
		ASpawnRod* spawnedRod = world->SpawnActor<ASpawnRod>(spawnRodBP, spawnLocation, rotator, spawnParams);
		rods.Add(spawnedRod);

	}
}
