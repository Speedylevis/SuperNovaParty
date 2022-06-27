// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SDuelGM.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASDuelGM : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	//call seamless travel back to the board level
	void BackToBoard();

private:

	//assign the pawns to the players involved in the duel
	void AssignPlayersToPawn();
};
