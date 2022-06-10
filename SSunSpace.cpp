// Fill out your copyright notice in the Description page of Project Settings.

#include "SSunSpace.h"

// ENGINE HEADERS
#include "Kismet/GameplayStatics.h"
#include "WidgetManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"

// CLASS HEADERS
#include "SPlayerPawn.h"
#include "SSunWidget.h"
#include "SSun.h"

void ASSunSpace::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASSunSpace, MovingPlayer);
	DOREPLIFETIME(ASSunSpace, LaneDestination);
	DOREPLIFETIME(ASSunSpace, bStartCompleted);
}

//
void ASSunSpace::MovePlayer(AActor* ThisMovingPlayer, int8 LaneNumber)
{
	if (ThisMovingPlayer && LaneNumber)
	{
		//store the moving player in a replicated variable
		MovingPlayer = ThisMovingPlayer;

		//set the lane destination based on the lane choice
		if (LaneNumber == 1)
		{
			LaneDestination = LaneOneStart;
		}
		else if (LaneNumber == 2)
		{
			LaneDestination = LaneTwoStart;
		}
		else if (LaneNumber == 3)
		{
			LaneDestination = LaneThreeStart;
		}
		else if (LaneNumber == 4)
		{
			LaneDestination = LaneFourStart;
		}
		else if (LaneNumber == 5)
		{
			LaneDestination = LaneFiveStart;
		}

		ASPlayerPawn* CurrentMover = Cast<ASPlayerPawn>(MovingPlayer);
		ASSpace* MoveDestination = Cast<ASSpace>(LaneDestination);

		//set the lane gate as the player's next space to move to
		SetNextSpace(MoveDestination);

		//move the player to the lane gate of their choice
		CurrentMover->BeginMoveForward();

		//decrement the amount of pawns on this space
		SetPawnCount(ReturnPawnCount() - 1);
	}
}