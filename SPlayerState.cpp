// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerState.h"

// CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SPlayerPawn.h"
#include "SSpace.h"
#include "SSun.h"

// ENGINE HEADERS
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//constructor to set the size of the card array
ASPlayerState::ASPlayerState(const FObjectInitializer& ObjectInitializer)
{ 
	PlayerCards.SetNum(3); 
	PlayerPaints.SetNum(3);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, PlayerPaints);
	DOREPLIFETIME(ASPlayerState, PlayerLane);
	DOREPLIFETIME(ASPlayerState, PlayerName);
	DOREPLIFETIME(ASPlayerState, PlayerCards);
	DOREPLIFETIME(ASPlayerState, PlayerScore);
	DOREPLIFETIME(ASPlayerState, PlayerPawn);
	DOREPLIFETIME(ASPlayerState, CurrentSpaceNum);
	DOREPLIFETIME(ASPlayerState, IsMyTurn);
	DOREPLIFETIME(ASPlayerState, NumOfTurns);
	DOREPLIFETIME(ASPlayerState, PlayerTurnNum);
	DOREPLIFETIME(ASPlayerState, IsInDuel);
	DOREPLIFETIME(ASPlayerState, IsDuelWinner);
	DOREPLIFETIME(ASPlayerState, PlayerColor);
}

//store properties to be maintained during Seamless Travel
void ASPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (PlayerState)
	{
		ASPlayerState* CustomPS = Cast<ASPlayerState>(PlayerState);
		if (CustomPS)
		{
			CustomPS->PlayerColor = PlayerColor;
			CustomPS->PlayerScore = PlayerScore;
			CustomPS->PlayerCards = PlayerCards;
			CustomPS->PlayerPaints = PlayerPaints;
			CustomPS->CurrentSpaceNum = CurrentSpaceNum;
			CustomPS->PlayerName = PlayerName;
			CustomPS->IsMyTurn = IsMyTurn;
			CustomPS->NumOfTurns = NumOfTurns;
			CustomPS->PlayerTurnNum = PlayerTurnNum;
			CustomPS->IsInDuel = IsInDuel;
			CustomPS->IsDuelWinner = IsDuelWinner;
		}
	}
}

//override properties with stored value after Seamless Travel
void ASPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	if (PlayerState)
	{
		ASPlayerState* CustomPS = Cast<ASPlayerState>(PlayerState);
		if (CustomPS)
		{
			PlayerColor = CustomPS->PlayerColor;
			PlayerScore = CustomPS->PlayerScore;
			PlayerCards = CustomPS->PlayerCards;
			PlayerPaints = CustomPS->PlayerPaints;
			CurrentSpaceNum = CustomPS->CurrentSpaceNum;
			PlayerName = CustomPS->PlayerName;
			IsMyTurn = CustomPS->IsMyTurn;
			NumOfTurns = CustomPS->NumOfTurns;
			PlayerTurnNum = CustomPS->PlayerTurnNum;
			IsInDuel = CustomPS->IsInDuel;
			IsDuelWinner = CustomPS->IsDuelWinner;
		}
	}
}

//assign the player's color
//TBD: eventually have this be a choice at the main menu
void ASPlayerState::SetPlayerColor(FLinearColor TheColor)
{
	if (PlayerPawn != nullptr)
	{
		PlayerColor = TheColor;
		ASPlayerPawn* ThisPawn = Cast<ASPlayerPawn>(PlayerPawn); 
		ThisPawn->SetPawnColor(PlayerColor);

		PlayerName = GetPlayerName();
	}
}

//when a player's score changes, have the sun check if a new threshold is crossed
void ASPlayerState::UpdatePlayerScore(float TheScore)
{
	PlayerScore += TheScore;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSun::StaticClass(), FoundActors);

	if(FoundActors.Num() > 0)
	{ Cast<ASSun>(FoundActors[0])->UpdateMaxPodCount(PlayerScore); }
}

//when returning to the board level, find the space the player was previously on and set them there
void ASPlayerState::AssignPlayerSpace()
{
	ASPlayerPawn* ThisPawn = Cast<ASPlayerPawn>(PlayerPawn);

	TArray<AActor*> FoundSpaces;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSpace::StaticClass(), FoundSpaces);

	//match the space's ID number with the one stored on each player's playerstate
	for (int i = 0; i < FoundSpaces.Num(); ++i)
	{
		ASSpace* ThisSpace = Cast<ASSpace>(FoundSpaces[i]);

		if (ThisSpace->ReturnSpaceNum() == CurrentSpaceNum)
		{
			ThisSpace->AssignPlayerToSpace(PlayerPawn);
			
			FTimerHandle NewTimerHandle;
			FTimerDelegate NewTimerDelegate;
			NewTimerDelegate.BindUFunction(ThisPawn, FName("SetPawnColor"), PlayerColor);

			GetWorldTimerManager().SetTimer(NewTimerHandle, NewTimerDelegate, 4.0f, false);
			break;
		}
	}

	//update the InGameWidget to reflect the current player turn and turn count
	if (GetWorld()->IsServer())
	{
		ASBoardGS* ThisGS = GetWorld()->GetGameState<ASBoardGS>();
		if (ThisGS)
		{
			FTimerHandle THandle_TurnWidget;
			GetWorldTimerManager().SetTimer(THandle_TurnWidget, ThisGS, &ASBoardGS::UpdateTurnWidgetOnPC, 6.0f, false);
		}
	}

	IsInDuel = false;
}