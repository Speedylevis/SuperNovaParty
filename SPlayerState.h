// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	ASPlayerState(const FObjectInitializer& ObjectInitializer);

	//SETTERS AND GETTERS

	//The Player's Score
	UFUNCTION(BlueprintCallable, Category = "Score")
	void UpdatePlayerScore(float TheScore);
	UFUNCTION(BlueprintCallable, Category = "Score")
	float ReturnPlayerScore() { return PlayerScore; };

	//The Player's Pawn
	void SetPlayerPawn(APawn* ThePawn) { PlayerPawn = ThePawn; };
	APawn* ReturnPlayerPawn() { return PlayerPawn; };

	//The Player's Color
	void SetPlayerColor(FLinearColor TheColor);
	FLinearColor ReturnPlayerColor() { return PlayerColor; };

	//The Player's Cards (randomized and updated from GameState)
	void SetPlayerCards(int CardIndex, FString CardToSet) { PlayerCards[CardIndex] = CardToSet; };
	TArray<FString> ReturnPlayerCards() { return PlayerCards; };
	// Specific Card (cards are indexed 1, 2, 3 from left to right)
	FString ReturnPlayerCard(int CIndex) { return PlayerCards[CIndex]; };

	//The Player's Paints
	void SetPlayerPaints(int PaintIndex, FString PaintColor) { PlayerPaints[PaintIndex] = PaintColor; };
	TArray<FString> ReturnPlayerPaints() { return PlayerPaints; };
	//Specific Paint (cards are indexed 1, 2, 3 from left to right)
	FString ReturnPlayerPaint(int PIndex) { return PlayerPaints[PIndex]; };

	//The Player's Space
	void SetCurrentSpaceNum(int SpaceToSet) { CurrentSpaceNum = SpaceToSet; };
	int ReturnCurrentSpaceNum() { return CurrentSpaceNum; };

	//The Player's Lane
	void SetPlayerLane(int8 ThisLane) { PlayerLane = ThisLane; };
	int8 ReturnPlayerLane() { return PlayerLane; };

	//If it is currently this player's turn
	void SetIsMyTurn(bool IsTurn) { IsMyTurn = IsTurn; };
	bool ReturnIsMyTurn() { return IsMyTurn; };

	//Stored reference for the number of turns left in the game
	void UpdateNumOfTurns(int NewTurnNum) { NumOfTurns = NewTurnNum; };
	int ReturnNumOfTurns() { return NumOfTurns; };

	//keep certain values persistent between level transitions
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OverrideWith(APlayerState* PlayerState) override;

	//find the space the player was on when returning to the board level
	void AssignPlayerSpace();

	void SetPlayerTurnNum(int8 PlayerTurn) { PlayerTurnNum = PlayerTurn; };
	int8 ReturnPlayerTurnNum() { return PlayerTurnNum; };

private:

	//variables to identify this player
	UPROPERTY(Replicated, VisibleAnywhere)
	FString PlayerName;

	UPROPERTY(Replicated, EditAnywhere)
	float PlayerScore = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere)
	int8 PlayerLane;

	UPROPERTY(Replicated, VisibleAnywhere)
	APawn* PlayerPawn;

	UPROPERTY(VisibleAnywhere, Meta = (HideAlphaChannel = true))
	FLinearColor PlayerColor;

	UPROPERTY(Replicated)
	int CurrentSpaceNum = 700;

	UPROPERTY(Replicated)
	bool IsMyTurn = false;

	UPROPERTY(Replicated)
	int NumOfTurns = 45;

	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<FString> PlayerCards;

	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<FString> PlayerPaints;

	UPROPERTY(Replicated)
	int8 PlayerTurnNum;
};
