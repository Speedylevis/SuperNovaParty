// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SBattle.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASBattle : public ASSpace
{
	GENERATED_BODY()

public:

	ASBattle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	//Call method on GM to move to 4player minigame
	UFUNCTION(Server, Reliable)
	void Battle();
};
