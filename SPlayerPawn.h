// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SPlayerPawn.generated.h"

class UBoxComponent;
class UCameraComponent;

UCLASS()
class COLLABTUT_API ASPlayerPawn : public APawn
{
	GENERATED_BODY()

public:

	//ENGINE METHODS

	// Sets default values for this pawn's properties
	ASPlayerPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// SETTERS AND GETTERS

	UFUNCTION()
	void SetPawnColor(FLinearColor ThisPawnColor);
	FLinearColor ReturnPawnColor() { return PawnColor; };

	void SetCurrentSpace(AActor* ThisCurrentSpace) { CurrentSpace = ThisCurrentSpace; };
	AActor* ReturnCurrentSpace() { return CurrentSpace; };

	// MOVEMENT METHODS

	void BeginMoveForward();

	void BeginMoveBackward();

	void Move();

	// ORBIT METHODS

	void Orbit();

	// SHIP METHODS

	UFUNCTION()
	virtual void OnRep_Color();

	void DeleteArray();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// TESTING

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player\|Components")
	UCameraComponent* TestCameraComp;

private:

	// SPACE METHODS AND VARIABLES

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player\|Space")
	AActor* CurrentSpace;

	// MOVEMENT VARIABLES

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player\|Movement")
	bool bIsMoving;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player\|Movement")
	int8 Moves;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player\|Movement")
	FVector TransitionPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Player\|Movement")
	float TransitionSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Player\|Movement");
	float RequiredDistanceToTransitionPoint;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player\|Movement")
	bool bIsCentripetal;

	// ORBIT VARIABLES

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player\|Orbit")
	bool bIsOrbiting;

	//speed at which player orbits their current space
	UPROPERTY(EditDefaultsOnly, Category = "Player\|Orbit");
	float DefaultOrbitSpeed;

	float CurrentOrbitSpeed;

	//the center of the space this player is orbiting
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player\|Orbit")
	FVector SpaceCenter;

	FVector OldSpaceCenter;

	//restricts orbit to XY plane
	FVector OrbitAxis;

	//speed of the ship when still in orbit but moving to the transition point
	UPROPERTY(EditDefaultsOnly, Category = "Player\|Orbit");
	float DeOrbitSpeed;

	// SHIP VARIABLES

	UPROPERTY(ReplicatedUsing = OnRep_Color, VisibleAnywhere, Category = "Player", Meta = (HideAlphaChannel = true))
	FLinearColor PawnColor;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player\|Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player\|Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Player\|Camera")
	float SpringArmLength = 2000.0f;

	UMaterialInstanceDynamic* NeonDynamicMaterialInst;

	UMaterialInstanceDynamic* AluminiumDynamicMaterialInst;

};