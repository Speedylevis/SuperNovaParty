// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	//player controller constructor
	ASPlayerController(const FObjectInitializer& ObjectInitializer);

	//allows keybinding of player input to methods
	virtual void SetupInputComponent();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//switch whether the player locators are on or off
	UFUNCTION()
	void SetShowPlayerLocations(bool whatToSet) { ShowPlayerLocations = whatToSet; };

	//turn on the player locators in the pause menu
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Indicators")
	bool ShowPlayerLocations = false;

	//method for getting the location and angle of objects when they are offscreen - CODE PROVIDED BY KNEEBITER ON THE UE4 FORUMS
	/**
	* Converts a world location to screen position for HUD drawing. This differs from the results of FSceneView::WorldToScreen in that it returns a position along the edge of the screen for offscreen locations
	*
	* @param InLocation - The world space location to be converted to screen space
	* @param EdgePercent - How close to the edge of the screen, 1.0 = at edge, 0.0 = at center of screen. .9 or .95 is usually desirable
	* @param ViewportCenterLoc - for offsetting center of the screen, leave at (0.5, 0.5) for no offset
	* @outparam OutScreenPosition - the screen coordinates for HUD drawing
	* @outparam OutRotationAngleDegrees - The angle to rotate a hud element if you want it pointing toward the offscreen indicator, 0° if onscreen
	* @outparam bIsOnScreen - True if the specified location is in the camera view (may be obstructed)
	*/
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext), Category = "HUD|Util")
	static void FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const float EdgePercent, const FVector2D ViewportCenterLoc, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool& bIsOnScreen);

	//widgets exist individually per client, so they each need to make one themselves
	UFUNCTION(Client, Reliable)
	void CreateManagerWidget(const TArray<FLinearColor>& WidgetColors);

	//remove all active widgets from the viewport
	UFUNCTION(Client, Reliable)
	void RemoveAllWidgets();	

	//have each client receive information to update the widget individually
	UFUNCTION(Client, Reliable)
	void UpdateTurnWidget(int TurnCount, const FString& PlayerTurn, int Whatever);

	//Switch widgets to the index of the passed value
	UFUNCTION(Client, Reliable)
	void ChangeWidgets(int WidgetToSwitch);

	//have each client update the widget with the winner info
	UFUNCTION(Client, Reliable)
	void UpdateWinningWidget(const FString& WinnerName, FLinearColor WinnerColor);

	//change active widget back to InGameWidget
	UFUNCTION(Server, Reliable)
	void WidgetCompleted();

	UFUNCTION(Server, Reliable)
	void MovePlayerReplicate(AActor* ThisPlayerPawn, int LaneDesintation);

	//determines whether this player controller can interact with objects on screen
	UFUNCTION(Client, Reliable)
	void SwitchInteractability(bool OnOrOFf);

	//determines whether this player controller can pause the game
	void SetAbleToBePaused(bool WhatToSet) { AbleToPause = WhatToSet; };
	void SetbIsPaused(bool GameIsPaused) { bIsPaused = GameIsPaused; };
	
	//replicate the score change from the bet at a gambler space
	UFUNCTION(Server, Reliable)
	void UpdateScoreFromGamblerSpace(int AmountToUpdate);

	//update widget text with text/color of duel choices
	UFUNCTION(Client, Reliable)
	void AddPlayersToDuelWidget(const FString& B1, FLinearColor C1, const FString& B2, FLinearColor C2, const FString& B3, FLinearColor C3);

	//the following methods are server RPCs called from client computers
	UFUNCTION(Server, Reliable)
	void SunWidgetCompleted();
	UFUNCTION(Server, Reliable)
	void UpdatePlayerSpaceNumbers(APawn* PawnToMove, int LaneToMoveTo, int CurrentLane);
	UFUNCTION(Server, Reliable)
	void UpdateLaneChoiceOnGS();
	UFUNCTION(Server, Reliable)
	void PassMouseInputToGS(int CardIndex, const FString &BoolToPass, bool WhatStatus); 
	UFUNCTION(Server, Reliable)
	void SunScalePassToGS();
	UFUNCTION(Server, Reliable)
	void TOCardChosen(const FString& ChosenCard);
	UFUNCTION(Server, Reliable)
	void CallDuelUpdate();
	UFUNCTION(Server, Reliable)
	void DuelChoicesPassToGS(const FString& Choice1, const FString& Choice2);
	UFUNCTION(Server, Reliable)
	void SetPlayerToDumpSpace();

private:

	//references to the widget classes of each PC
	TSubclassOf<class UUserWidget> ManagerClass;
	class UWidgetManager* ManagerWidget;
	class UInGameWidget* InGameWidget;
	class USScoreWidget* ScoreWidget;
	class USWinWidget* WinWidget;
	class USDuelChoice* DuelWidget;

	//player pawn
	APawn* PlayerPawn;

	//method for pausing the game
	UFUNCTION(Server, Reliable)
	void PauseGame();

	//bool for recognizing if the game is currently paused or not
	bool bIsPaused = false;

	//bool for acknowledging if this player is currently capable of pausing the game
	bool AbleToPause = false;

	//arrays for the output of the location indicator method
	UPROPERTY(Replicated)
	TArray<FLinearColor> Indicator_Color;
	UPROPERTY(Replicated)
	TArray<FVector2D> Indicator_OutPos;
	UPROPERTY(Replicated)
	TArray<float> Indicator_OutAngle;
	UPROPERTY(Replicated)
	TArray<bool> Indicator_OnScreen;
	
	//update the arrays on the server to replicate them to all computers
	UFUNCTION(Server, Reliable)
	void UpdateLocatorInformation();

	//take updated arrays and update the location of the indicators on the widgets of each computer
	UFUNCTION(NetMulticast, Reliable)
	void UpdateLocatorsOnWidget(const TArray<FLinearColor>& IColor, const TArray<FVector2D>& IOutPos, const TArray<bool>& IOnScreen);
};
