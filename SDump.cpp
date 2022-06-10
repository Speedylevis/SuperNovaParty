// Fill out your copyright notice in the Description page of Project Settings.


#include "SDump.h"

ASDump::ASDump()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

//send a player to the dump if they land on this space
void ASDump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		bIsActive = false;
		DumpPlayer();
		OrbitingPlayer = nullptr;
		NextTurn();
	}
}