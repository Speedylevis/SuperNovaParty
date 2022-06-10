// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SSunSpace.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASSunSpace : public ASSpace
{
	GENERATED_BODY()
	
public:

	//move the current player to their chosen lane
	void MovePlayer(AActor* ThisMovingPlayer, int8 LaneDestination);

	//set the bool after finishing the initial lane choice at the start of the game
	void SetStartCompleted(bool IsCompleted) { bStartCompleted = IsCompleted; };

private:

	//bool to know if this lane choice is the initial one at the start of the game
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Space")
	bool bStartCompleted = false;

	//actor reference to replicate the movement of the player
	UPROPERTY(Replicated, VisibleAnywhere)
	AActor* MovingPlayer;

	//the lane gate of the player's lane choice
	UPROPERTY(Replicated, VisibleAnywhere)
	AActor* LaneDestination;

	//actor reference to the lane one gate
	UPROPERTY(EditInstanceOnly, Category = "Lane Starts")
	AActor* LaneOneStart;

	//actor reference to the lane two gate
	UPROPERTY(EditInstanceOnly, Category = "Lane Starts")
	AActor* LaneTwoStart;

	//actor reference to the lane three gate
	UPROPERTY(EditInstanceOnly, Category = "Lane Starts")
	AActor* LaneThreeStart;

	//actor reference to the lane four gate
	UPROPERTY(EditInstanceOnly, Category = "Lane Starts")
	AActor* LaneFourStart;

	//actor reference to the lane five gate
	UPROPERTY(EditInstanceOnly, Category = "Lane Starts")
	AActor* LaneFiveStart;
};
