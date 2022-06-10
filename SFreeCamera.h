// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SFreeCamera.generated.h"

UCLASS()
class COLLABTUT_API ASFreeCamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASFreeCamera();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	//methods called when player provides keyboard input
	UFUNCTION(Server, Reliable)
	void MoveUp(float Value);
	UFUNCTION(Server, Reliable)
	void MoveRight(float Value);

	//values to determine movement of camera based on player input
	UPROPERTY(Replicated, EditAnywhere, Category = "Camera Movement")
	float MoveSpeed = 20.0f;
	UPROPERTY(Replicated)
	FVector MovementDirection;
};
