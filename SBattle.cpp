// Fill out your copyright notice in the Description page of Project Settings.


#include "SBattle.h"

//CLASS HEADERS
#include "BoardGM.h"

ASBattle::ASBattle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASBattle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		Battle();
	}
}

//Call method on GM to move to 4player minigame
void ASBattle::Battle_Implementation()
{
	bIsActive = false;
	
	if (GetWorld()->IsServer())
	{
		ABoardGM* ThisGM = GetWorld()->GetAuthGameMode<ABoardGM>();
		FTimerHandle THandle_MinigameMove;
		GetWorldTimerManager().SetTimer(THandle_MinigameMove, ThisGM, &ABoardGM::MoveToMinigame, 2.0f, false);
	}
}