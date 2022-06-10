// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SBoardGS.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASBoardGS : public AGameStateBase
{
	GENERATED_BODY()

public:

	//constructor
	ASBoardGS(const FObjectInitializer& ObjectInitializer);

	//update the chosen turn order card on all servers/clients
	UFUNCTION(NetMultiCast, Reliable)
	void TOCardChosen(const FString& ChosenCard, const FString& PName);

	//bool to check if the game is in initial setup or is returning from a level transition
	bool ReturnPostMinigame() { return ReturningFromMinigame; };

	//Spawn the cards on both the server and client
	UFUNCTION(NetMultiCast, Reliable)
	void CardSpawning(const FString &C1, const FString &C2, const FString &C3, const FString &P1, const FString &P2, const FString &P3);

	//receive the function call from player controller to update cards on all clients and server
	UFUNCTION(NetMultiCast, Reliable)
	void ReplicateBoolToCards(int CIndex, const FString& BoolToUpdate, bool SetStatus);

	//setter and getter for whether the RedCard has been played
	bool ReturnRedCardActive() { return RedCardActivated; };
	void SetRedCardActive(bool CardIsActive) { RedCardActivated = CardIsActive; };

	//move the current pawn based on the chosen card
	void PawnMovement(int MovementNumber);

	//finish a player's turn and update the UI and PC for the next player
	void NextTurnInOrder();

	//refresh the array of player scores
	void UpdatePlayerScoreArray(FString PlayerToUpdate, float ScoreToUpdate);

	//have the sun expand on all server/clients
	UFUNCTION(NetMultiCast, Reliable)
	void UpdateNovaSun();

	//access the player who's turn it currently is
	int ReturnCurrentPlayerTurn() { return CurrentPlayerTurn; };

	//access the array of all player's current scores
	TArray<float> ReturnScoreArray() { return PlayerScoreArray; };

	//pause or unpause the game based on the given bool
	void PMenuSwitch(bool IsGamePaused);

	//take action based on if a player has made their lane choice or is landing at their lane choice
	UFUNCTION(Server, Reliable)
	void LaneChoice(bool LeavingOrLanding);

	//allow the next player to pick make their lane choice from the sun space, and move to next turn when all players have left the sun
	void NextOnSunWidget();

	//update the widgets after the gamble space minigame is completed
	UFUNCTION(NetMultiCast, Reliable)
	void ReplicateWidgetCompleted();

	//method to end the game
	void InitiateWinScreen();

	//method to access and return the value of each planet
	TArray<int> ReturnPlanetScores();

	//method to have the cameras of all players focus on the passed in actor
	void FocusCameraOnActor(AActor* ActorToFocusOn, float TimeToBlend);

	//update the widgets on each client with turn order and current player turn
	void UpdateTurnWidgetOnPC();
	
	//method to move a player to another lane based on their choice at a wormhole space
	void WormHoleMovement(APawn* PawnToMove, int LaneToMoveTo, int CurrentLane);

	//setters and getters to track if a player is landing on a wormhole space or is using a wormhole space to change lanes
	void SetIsOnWormhole(bool OnWormhole) { IsOnWormhole = OnWormhole; };
	bool ReturnIsOnWormhole() { return IsOnWormhole; };

private:

	//tracker for how many turns are left in the game
	UPROPERTY(Replicated, VisibleAnywhere)
	int TurnCount = 46;

	//bool to track if the players are picking their initial lanes or if a player is returning to the SunSpace after reaching the end of a lane
	bool StartingTurn = true;

	//tracker for which player is currently in control
	UPROPERTY(Replicated, VisibleAnywhere)
	int CurrentPlayerTurn;
	//order of array represents current player turn order
	TArray<int> TurnOrderArray;

	//replicated array for to randomize the order of the turn order cards
	UPROPERTY(Replicated)
	TArray<FString> PlayerTurnOrder;

	//track the number of turn order cards chosen to know when all 4 are picked
	UPROPERTY(Replicated)
	int TOCardCounter = 0;

	//the turn order camera pawn blueprint, drag and drop on GS blueprint
	UPROPERTY(EditDefaultsOnly, Category = "ActorsToSpawn")
	TSubclassOf<class APawn> TOCameraPawnClass;

	//re-order the turn order array
	UFUNCTION(BlueprintCallable)
	void InitialTurnOrder();

	//method to have all the players possess the turn order camera pawn
	void PossessTOCameraPawn();
	//reference variable for the turn order camera pawn
	APawn* CameraPawn;

	//method to lienarly assign turn order instead of playing the minigame
	UFUNCTION(BlueprintCallable)
	void SkipTurnOrderSetup();

	//all actions that are performed on the map loading in
	UFUNCTION(BlueprintCallable)
	void InitialSetup();

	//store the initial values to define each player state
	void InitialPlayerStates();

	//bool to track if the game is in initial setup or is returning from a level transition
	UPROPERTY(Replicated)
	bool ReturningFromMinigame = false;

	//method to return the game to its previous setup after returning from a level transition
	void PostMinigameSetup();

	//references to the 3 cards of the current player
	class ASCardSystem* Card_1;
	class ASCardSystem* Card_2;
	class ASCardSystem* Card_3;

	//spawn locations for the cards based on the current camera viewpoint
	FVector CardLocation_1;
	FVector CardLocation_2;
	FVector CardLocation_3;

	//class reference for the card blueprint
	UPROPERTY(EditDefaultsOnly, Category = "ActorsToSpawn")
	TSubclassOf<class ASCardSystem> CardSystemClass;

	//the movement card blueprint, drag and drop on GS blueprint
	UPROPERTY(EditDefaultsOnly, Category = "ActorsToSpawn")
	TSubclassOf<class ASTOCards> CardBP;

	//array to hold the cards of the current player for spawning
	TArray<FString> CurrentCards;

	//array to hold the paints of the current players cards
	TArray<FString> CurrentPaints;

	//array to hold the 4 turn order cards
	TArray<AActor*> TOCards;

	//bool to track if the RedCard has been activated
	bool RedCardActivated = false;

	//randomly choose a card type and then randomly choose a card of that type
	FString CardRandomizer();

	//assign the cards of the player who's turn it currently is
	void CurrentCardSetup();

	//the player pawn ship blueprint, drag and drop on the GS blueprint 
	UPROPERTY(EditDefaultsOnly, Category = "ActorsToSpawn")
	TSubclassOf<class APawn> PlayerShipClass;

	//array of the player ships
	UPROPERTY(Replicated)
	TArray<APawn*> PlayerPawns;

	//possess the ship of the player who's turn it currently is
	void PossessPlayerPawns();

	//method to change the camera of all players to the space of the current player
	void SetViewToCurrentSpace();

	//array to match the player numbers with their chosen colors
	UPROPERTY(Replicated)
	TArray<FLinearColor> ColorArray;

	//array to match the player numbers with their current scores
	UPROPERTY(Replicated)
	TArray<float> PlayerScoreArray;

	//reference variable for the pause menu camera pawn
	APawn* FreeCamPawn;

	//class reference to the camera controlled during pause
	UPROPERTY(EditDefaultsOnly, Category = "Free Camera")
	TSubclassOf<class APawn> FreeCameraClass;

	//possessions and destructions need to be delayed to allow for camera control to blend
	void DestroyFreeCam();
	void DelayedCameraSpawn();

	//bool to prevent methods from rerunning when moving a player after using a wormhole space
	UPROPERTY(Replicated)
	bool IsOnWormhole = false;

	//array to store the value of each planet
	TArray<int> PlanetCounts;
};
