// Fill out your copyright notice in the Description page of Project Settings.


#include "SFreeCamera.h"

//ENGINE HEADERS
#include "Net/UnrealNetwork.h"

// Sets default values
ASFreeCamera::ASFreeCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ASFreeCamera::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASFreeCamera, MoveSpeed);
	DOREPLIFETIME(ASFreeCamera, MovementDirection);
}

// Called every frame
void ASFreeCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//update the position of this actor by the variable if it is ever changed
	if (!MovementDirection.IsZero())
	{
		FVector NewLocation = GetActorLocation() + (MovementDirection * DeltaTime * MoveSpeed);
		SetActorLocation(NewLocation);
	}
}

// Called to bind functionality to input
void ASFreeCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASFreeCamera::MoveUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASFreeCamera::MoveRight);
}

//update variable on server to have it replicate and trigger the method in Tick on all computers
void ASFreeCamera::MoveUp_Implementation(float Value)
{ MovementDirection.X = FMath::Clamp(Value, -1.0f, 1.0f); }

void ASFreeCamera::MoveRight_Implementation(float Value)
{ MovementDirection.Y = FMath::Clamp(Value, -1.0f, 1.0f); }
