// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FlyingTest1Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"

AFlyingTest1Pawn::AFlyingTest1Pawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());	// Set static mesh
	//RootComponent = PlaneMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);	// Attach SpringArm to RootComponent
	SpringArm->TargetArmLength = ArmLength; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,CameraHeight);
	SpringArm->bEnableCameraLag = false;	// Do not allow camera to lag
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach the camera
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 3000.f;
	TurnSpeed = 100.f;
	MaxSpeed = 4000.f;
	MinSpeed = 0.f;
	CurrentForwardSpeed = 500.f;

	g = 9.81;
}

void AFlyingTest1Pawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	if (AirRollOn)
	{
		FTransform planeTransform = PlaneMesh->GetComponentTransform();
		//FVector planeLocation = planeTransform.GetLocation();
		FRotator planeRotation = planeTransform.GetRotation().Rotator();

		//FVector planeScaledBounds = getScaledBounds(PlaneMesh);
		//float mass = PlaneMesh->GetMass();
		float velocityForward = CurrentForwardSpeed * DeltaSeconds;
		printToScreenDebug(planeRotation.Vector());
		//FVector velocityFromComponent = PlaneMesh->GetComponentVelocity();
		//printToScreenDebug(velocityFromComponent);
		float x = FMath::Cos(planeRotation.Vector().X)*velocityForward;
		float y = FMath::Cos(planeRotation.Vector().Y)*velocityForward;
		float z = FMath::Cos(planeRotation.Vector().Z)*velocityForward;
		FVector velocityComponents = FVector(x, y, z);

		//PlaneMesh->AddWorldOffset(velocityFromComponent, true);
		//PlaneMesh->AddWorldTransform(FTransform(FRotator(0), velocityComponents, FVector(0)));

		RootComponent->AddLocalOffset(LocalMove, true);
	}
	else
	{
		RootComponent->AddLocalOffset(LocalMove, true);
	}
	
	//const FVector LocalMove = FVector(0.f, 0.f, CurrentForwardSpeed * DeltaSeconds);
	//const FVector LocalMove = FVector(0.f, CurrentForwardSpeed * DeltaSeconds, 0.f);

	// Move plan forwards (with sweep so we stop when we collide with things)
	

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	//AddActorLocalRotation(DeltaRotation);
	PlaneMesh->AddRelativeRotation(DeltaRotation);

	if (!AirRollOn)
	{
		RootComponent->SetWorldRotation(FMath::Lerp(RootComponent->GetComponentRotation(), PlaneMesh->GetComponentRotation(), 4));
	}

	float F = PlaneMesh->GetMass() * g;
	float velocityVectorLength = PlaneMesh->GetComponentVelocity().Size();
	float lerpedGravity = FMath::Lerp(0.f, F, velocityVectorLength);

	PlaneMesh->AddForce(FVector(0, 0, lerpedGravity));
	
	//PlaneMesh->AddForceAtLocationLocal();
	//PlaneMesh->AddForceAtLocation()

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AFlyingTest1Pawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}


void AFlyingTest1Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("Thrust", this, &AFlyingTest1Pawn::ThrustInput);
	PlayerInputComponent->BindAxis("MoveUp", this, &AFlyingTest1Pawn::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFlyingTest1Pawn::MoveRightInput);
	PlayerInputComponent->BindAction("CameraLock", IE_Pressed, this, &AFlyingTest1Pawn::CameraLockToggle);

	PlayerInputComponent->BindAction("AirRoll", IE_Pressed, this, &AFlyingTest1Pawn::OnAirRollPress);
	PlayerInputComponent->BindAction("AirRoll", IE_Released, this, &AFlyingTest1Pawn::OnAirRollRelease);
}

void AFlyingTest1Pawn::ThrustInput(float Val)
{
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void AFlyingTest1Pawn::MoveUpInput(float Val)
{
	if (mirroredVertical) Val = -Val;

	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	if (!AirRollOn)
	{
		// When steering, we decrease pitch slightly
		TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

		// Smoothly interpolate to target pitch speed
		CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
	}
	else
	{
		PlaneMesh->AddRelativeRotation(FRotator(airRollAmount*Val*0.4, 0, 0));

	}

	

	
}

void AFlyingTest1Pawn::MoveRightInput(float Val)
{
	if (AirRollOn)
	{
		// Air rolling
		//PlaneMesh->AddLocalRotation(FRotator(0, 0, airRollAmount*Val));
		PlaneMesh->AddRelativeRotation(FRotator(0, 0, airRollAmount*Val));
		//printToScreenDebug("Air Rolling");
	}
	else
	{
		// Target yaw speed is based on input
		float TargetYawSpeed = (Val * TurnSpeed);

		// Smoothly interpolate to target yaw speed
		CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

		// Is there any left/right input?
		const bool bIsTurning = FMath::Abs(Val) > 0.2f;

		// If turning, yaw value is used to influence roll
		// If not turning, roll to reverse current roll value.
		float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

		// Smoothly interpolate roll speed
		CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
	}	
}

void AFlyingTest1Pawn::CameraLockToggle()
{
	if (SpringArm->bInheritPitch)
	{
		// Camera was locked before
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;

		SpringArm->SetRelativeRotation(PlaneMesh->GetComponentRotation());

		//printToScreenDebug(lastCameraLockRotation.Vector());
	}
	else
	{
		// Camera was not locked before

		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;

		FRotator currentRotation = PlaneMesh->GetComponentRotation();
		//printToScreenDebug(currentRotation.Vector());

		SpringArm->SetRelativeRotation(FRotator(0));
		//SpringArm->AddLocalRotation(FRotator(currentRotation.Pitch - lastCameraLockRotation.Pitch, currentRotation.Yaw - lastCameraLockRotation.Yaw, currentRotation.Roll - lastCameraLockRotation.Roll));
	}
}

void AFlyingTest1Pawn::OnAirRollPress()
{
	FRotator currentRotation = PlaneMesh->GetComponentRotation();
	//currentRotation.Normalize();
	float rotationX = (FMath::Clamp(currentRotation.Pitch, -180.f, 180.0f));
	float rotationY = (FMath::Clamp(currentRotation.Yaw, -180.f, 180.0f));
	float rotationZ = (FMath::Clamp(currentRotation.Roll, -180.f, 180.0f));
	//printToScreenDebug(rotationX);
	//printToScreenDebug(rotationY);
	//printToScreenDebug(rotationZ);
	lastRotationRadians = FVector(rotationX, rotationY, rotationZ);
	AirRollOn = true;
}

void AFlyingTest1Pawn::OnAirRollRelease()
{
	AirRollOn = false;
}






FVector AFlyingTest1Pawn::getScaledBounds(UStaticMeshComponent * meshComponent)
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

void AFlyingTest1Pawn::printToScreenDebug(FTransform transform)
{
	FVector transformVector;
	transform.TransformVector(transformVector);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Ground transform vector: X > %f, Y > %f, Z > %f"), transformVector.X, transformVector.Y, transformVector.Z));
}

void AFlyingTest1Pawn::printToScreenDebug(FVector vector)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Vector: X > %f, Y > %f, Z > %f"), vector.X, vector.Y, vector.Z));
}

void AFlyingTest1Pawn::printToScreenDebug(float floatVal)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("float: %f"), floatVal));
}

void AFlyingTest1Pawn::printToScreenDebug(FString stringIn)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, stringIn);
}
