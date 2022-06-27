// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SDuelGS.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASDuelGS : public AGameStateBase
{
	GENERATED_BODY()

public:

	//assign the duel winner and call travel back to the board
	UFUNCTION(Server, Reliable)
	void DuelOver(const FString& LosingPlayer);
};
