// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlyingTest1Pawn.generated.h"

UCLASS(Config=Game)
class AFlyingTest1Pawn : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */

	//class UStaticMeshComponent;

	//UPROPERTY()
	//TSubobjectPtr<class USceneComponent> DummyRoot;

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* PlaneMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
public:
	AFlyingTest1Pawn();

	// Begin AActor overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

protected:

	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	// End APawn overrides

	/** Bound to the thrust axis */
	void ThrustInput(float Val);
	
	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	/** Camera Lock toggle */
	void CameraLockToggle();

	/** Air Roll functions */
	void OnAirRollPress();
	void OnAirRollRelease();

private:

	/** How quickly forward speed changes */
	UPROPERTY(Category = FlightSettings, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category = FlightSettings, EditAnywhere)
	float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = FlightSettings, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = FlightSettings, EditAnywhere)
	float MinSpeed;

	/** Current forward speed */
	float CurrentForwardSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

	/** Will be set to the current state of the air roll key*/
	bool AirRollOn;

	UPROPERTY(EditAnywhere, Category = FlightSettings)
	float airRollAmount = 5.0f;

	UPROPERTY(EditAnywhere, Category = FlightSettings)
	bool mirroredVertical = false;

	UPROPERTY(EditAnywhere, Category=CameraSettings)
	float ArmLength = 400.f;

	UPROPERTY(EditAnywhere, Category = CameraSettings)
	float CameraHeight = 100.0f;


	/** Gravity acceleration (g)*/
	UPROPERTY(EditAnywhere)
	float g;

	// Gotten from MapGenerate.h
	FVector getScaledBounds(UStaticMeshComponent* meshComponent);


	FQuat lastRotation;



	void printToScreenDebug(FTransform transform);
	void printToScreenDebug(FVector vector);
	void printToScreenDebug(float floatVal);
	void printToScreenDebug(FString stringIn);

public:
	/** Returns PlaneMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
};
