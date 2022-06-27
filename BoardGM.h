// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoardGM.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ABoardGM : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	//Call seamless travel to move to the 4player minigame level
	UFUNCTION(BlueprintCallable)
	void MoveToMinigame();
	
	//Call seamless travel to move to the 2player duel level
	UFUNCTION(BlueprintCallable)
	void MoveToDuel();

};
