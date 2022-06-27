// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardGM.h"

//ENGINE HEADERS
#include "Net/UnrealNetwork.h"

//CLASS HEADERS
#include "SPlayerController.h"
#include "SPlayerState.h"

//Call seamless travel to move to 4player minigame level
void ABoardGM::MoveToMinigame()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/Blockout_P?listen");
}

//Call seamless travel to move to 2player duel level
void ABoardGM::MoveToDuel()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/DuelLevel?listen");
}
