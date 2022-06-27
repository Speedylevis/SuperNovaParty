// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

//ENGINE HEADERS
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include <GameFramework/Character.h>
#include <Engine/Engine.h>

//CLASS HEADERS
#include "SBoardGS.h"
#include "InGameWidget.h"
#include "SCardSystem.h"
#include "SDuelChoice.h"
#include "SPlayerPawn.h"
#include "SPlayerState.h"
#include "SScoreWidget.h"
#include "SSunSpace.h"
#include "SSunWidget.h"
#include "SWinWidget.h"
#include "WidgetManager.h"


ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer)
{
	//access the widget manager, uses a widget switcher to alternate between different widgets
	ConstructorHelpers::FClassFinder<UUserWidget> WidgetManagerClass(TEXT("/Game/Widgets/WBP_WidgetManager"));
	if (!ensure(WidgetManagerClass.Class != nullptr)) return;
	ManagerClass = WidgetManagerClass.Class;

	//set the size of the arrays for the location indicators
	Indicator_Color.SetNum(4);
	Indicator_OutPos.SetNum(4);
	Indicator_OutAngle.SetNum(4);
	Indicator_OnScreen.SetNum(4);
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerController, Indicator_Color);
	DOREPLIFETIME(ASPlayerController, Indicator_OutPos);
	DOREPLIFETIME(ASPlayerController, Indicator_OutAngle);
	DOREPLIFETIME(ASPlayerController, Indicator_OnScreen);
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	//when "p" is pressed, pause or unpause the game
	InputComponent->BindAction("PauseGame", IE_Pressed, this, &ASPlayerController::PauseGame);
}

//pause game to bring up score and free cam, unpause to return to player's turn
void ASPlayerController::PauseGame_Implementation()
{
	//if the player is able to pause/unpause the game, pass the information to the GameState and NetMultiCast it
	if (AbleToPause)
	{
		AbleToPause = false;

		ASBoardGS* ThisGS = Cast<ASBoardGS>(GetWorld()->GetGameState());
		ThisGS->PMenuSwitch(bIsPaused);
	}
}

void ASPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if the game is paused, update the information of the location indicators every tick
	if (ShowPlayerLocations)
	{ UpdateLocatorInformation(); }
}

//use the locator method to get outputs for the information on where the location indicators should be placed
void ASPlayerController::UpdateLocatorInformation_Implementation()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASPlayerPawn::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		for (int i = 0; i < FoundActors.Num(); ++i)
		{
			Indicator_Color[i] = Cast<ASPlayerPawn>(FoundActors[i])->ReturnPawnColor();

			FindScreenEdgeLocationForWorldLocation(GetWorld(), FoundActors[i]->GetActorLocation(), 0.9f, FVector2D(0.5f, 0.5f), Indicator_OutPos[i], Indicator_OutAngle[i], Indicator_OnScreen[i]);
		}

		UpdateLocatorsOnWidget(Indicator_Color, Indicator_OutPos, Indicator_OnScreen);
	}
}

//use the color, position, and whether the object is on screen to place the appropriate locator in the correct location
void ASPlayerController::UpdateLocatorsOnWidget_Implementation(const TArray<FLinearColor>& IColor, const TArray<FVector2D>& IOutPos, const TArray<bool>& IOnScreen)
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	UWidgetManager* TheManager = Cast<UWidgetManager>(FoundWidgets[0]);
	ScoreWidget = Cast<USScoreWidget>(TheManager->SwitchReturn());

	for (int i = 0; i < 4; ++i)
	{ ScoreWidget->UpdateLocationIndicators(IColor[i], IOutPos[i], IOnScreen[i]); }
}

//use the screen's size and the location of an object to find the closest spot on screen to where they are
void ASPlayerController::FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const float EdgePercent, const FVector2D ViewportCenterLoc, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool& bIsOnScreen)
{
	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;
	FVector2D ScreenPosition = FVector2D();

	if (!GEngine)
	{ return; }

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D ViewportCenter = FVector2D(ViewportSize.X * ViewportCenterLoc.X, ViewportSize.Y * ViewportCenterLoc.Y);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);

	if (!World)
	{ return; }

	APlayerController* PlayerController = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);

	if (!PlayerController)
	{ return; }

	FVector CameraLoc;
	FRotator CameraRot;

	PlayerController->GetPlayerViewPoint(CameraLoc, CameraRot);

	const FVector CameraToLoc = InLocation - CameraLoc;
	FVector Forward = CameraRot.Vector();
	FVector Offset = CameraToLoc.GetSafeNormal();

	float DotProduct = FVector::DotProduct(Forward, Offset);
	bool bLocationIsBehindCamera = (DotProduct < 0);

	if (bLocationIsBehindCamera)
	{
		FVector Inverted = CameraToLoc * -1.f;
		FVector NewInLocation = CameraLoc + Inverted;

		PlayerController->ProjectWorldLocationToScreen(NewInLocation, ScreenPosition);

		ScreenPosition.X = ViewportSize.X - ScreenPosition.X;
		ScreenPosition.Y = ViewportSize.Y - ScreenPosition.Y;
	}
	else
	{
		PlayerController->ProjectWorldLocationToScreen(InLocation, ScreenPosition);
	}

	// Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.
	if (ScreenPosition.X >= 0.f && ScreenPosition.X <= ViewportSize.X
		&& ScreenPosition.Y >= 0.f && ScreenPosition.Y <= ViewportSize.Y && !bLocationIsBehindCamera)
	{
		OutScreenPosition = ScreenPosition;
		bIsOnScreen = true;
		return;
	}

	ScreenPosition -= ViewportCenter;

	float AngleRadians = FMath::Atan2(ScreenPosition.Y, ScreenPosition.X);
	AngleRadians -= FMath::DegreesToRadians(90.f);

	OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;

	float Cos = cosf(AngleRadians);
	float Sin = -sinf(AngleRadians);

	ScreenPosition = FVector2D(ViewportCenter.X + (Sin * 150.f), ViewportCenter.Y + Cos * 150.f);

	float m = Cos / Sin;

	FVector2D ScreenBounds = ViewportCenter * EdgePercent;

	if (Cos > 0)
	{ ScreenPosition = FVector2D(ScreenBounds.Y / m, ScreenBounds.Y); }
	else
	{ ScreenPosition = FVector2D(-ScreenBounds.Y / m, -ScreenBounds.Y); }

	if (ScreenPosition.X > ScreenBounds.X)
	{ ScreenPosition = FVector2D(ScreenBounds.X, ScreenBounds.X * m); }
	else if (ScreenPosition.X < -ScreenBounds.X)
	{ ScreenPosition = FVector2D(-ScreenBounds.X, -ScreenBounds.X * m); }

	ScreenPosition += ViewportCenter;

	OutScreenPosition = ScreenPosition;
}

//create the WidgetManager for switching between widgets on the board state
void ASPlayerController::CreateManagerWidget_Implementation(const TArray<FLinearColor>& WidgetColors)
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	if (FoundWidgets.Num() > 0)
	{
		for (int i = 0; i < FoundWidgets.Num(); ++i)
		{ FoundWidgets[i]->RemoveFromViewport(); }
	}

	//Create the manager widget and store the widget at index 0
	if (!ensure(ManagerClass != nullptr)) return;
	ManagerWidget = CreateWidget<UWidgetManager>(this, ManagerClass);
	if (!ensure(ManagerWidget != nullptr)) return;
	ManagerWidget->AddToViewport();
	ManagerWidget->bIsFocusable = true;

	//right now there is only one widget, it displays the turn order and current player turn
	ScoreWidget = Cast<USScoreWidget>(ManagerWidget->SwitchReturn());

	//set the fonts for the score widget based on each player's color choice
	ScoreWidget->UpdatePlayerColors(WidgetColors);

	//if the returning from a minigame, set the InGameWiget
	if (GetWorld()->GetGameState<ASBoardGS>()->ReturnPostMinigame() || GetWorld()->GetGameState<ASBoardGS>()->ReturnPostDuel())
	{ ManagerWidget->SwitchWidgets(1); }
	//if not returning from a minigame, it is the start so set the SunWidget
	else
	{ ManagerWidget->SwitchWidgets(3); }
}

//remove all active widgets from the viewport
void ASPlayerController::RemoveAllWidgets_Implementation()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	if (FoundWidgets.Num() > 0)
	{
		for (int i = 0; i < FoundWidgets.Num(); ++i)
		{
			FoundWidgets[i]->RemoveFromViewport();
		}
	}
}

//at the end of a player's movement, update the widget with the current player turn and change in turn order
void ASPlayerController::UpdateTurnWidget_Implementation(int TurnCount, const FString& PlayerTurn, int Whatever)
{
	//look for all widgets on screen, by using a manager with a widget switcher we will only have one widget on scree and so we are able to use this function
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	//access the current widget on screen, the ingame widget
	UWidgetManager* TheManager = Cast<UWidgetManager>(FoundWidgets[0]);
	InGameWidget = Cast<UInGameWidget>(TheManager->SwitchReturn());

	//update the variables passed from the GameState
	InGameWidget->SetPlayerTurn(PlayerTurn);
	InGameWidget->SetTurnNumber(FString::FromInt(TurnCount));
}

//change the widgets based on the passed index value
void ASPlayerController::ChangeWidgets_Implementation(int WidgetToSwitch)
{
	//look for all widgets on screen, by using a manager with a widget switcher we will only have one widget on scree and so we are able to use this function
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	//if there are additional widgets created, delete all but the initial one
	if (FoundWidgets.Num() > 1)
	{
		for (int i = 1; i < FoundWidgets.Num(); ++i)
		{
			FoundWidgets[i]->RemoveFromViewport();
		}
	}

	//access the widgt manager and switch the widgets based on the context
	UWidgetManager* TheManager = Cast<UWidgetManager>(FoundWidgets[0]);
    
	TheManager->SwitchWidgets(WidgetToSwitch); 

	//if the widget is switching to the Score Widget, grab the scores from all players to update on each widget
	if (WidgetToSwitch == 0)
	{	
		TArray<float> ThePlayerScores = Cast<ASBoardGS>(GetWorld()->GetGameState())->ReturnScoreArray();

		ScoreWidget = Cast<USScoreWidget>(TheManager->SwitchReturn());
		ScoreWidget->UpdatePlayersScores(ThePlayerScores);
	}
}

//update the widget to represent the winner of the game
void ASPlayerController::UpdateWinningWidget_Implementation(const FString& WinnerName, FLinearColor WinnerColor)
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	UWidgetManager* TheManager = Cast<UWidgetManager>(FoundWidgets[0]);

	WinWidget = Cast<USWinWidget>(TheManager->SwitchReturn());

	WinWidget->UpdateWinningText(WinnerName, WinnerColor);
}

//reset the onscreen widgets to the InGameWidget for turn order and turn count
void ASPlayerController::WidgetCompleted_Implementation()
{
	ASBoardGS* ThisGS = GetWorld()->GetGameState<ASBoardGS>();
	FTimerHandle THandle_WidgetCompleteNextTurn;

	ThisGS->ReplicateWidgetCompleted();
	GetWorldTimerManager().SetTimer(THandle_WidgetCompleteNextTurn, ThisGS, &ASBoardGS::NextTurnInOrder, 1.0f, false);
}

//move player pawns based on the lane choice at the SunSpace
void ASPlayerController::MovePlayerReplicate_Implementation(AActor* ThisPlayerPawn, int LaneDesintation)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSunSpace::StaticClass(), FoundActors);

	ASSunSpace* TheSun = Cast<ASSunSpace>(FoundActors[0]);
	TheSun->MovePlayer(ThisPlayerPawn, LaneDesintation);

	ASBoardGS* ThisGS = Cast<ASBoardGS>(GetWorld()->GetGameState());

	//match the player name to the current player turn and move that ship based on the lane chosen
	for (int i = 0; i < ThisGS->PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(ThisGS->PlayerArray[i]);

		if (ThisGS->ReturnCurrentPlayerTurn() == 1 && ThisPS->GetPlayerName() == "P1" ||
			ThisGS->ReturnCurrentPlayerTurn() == 2 && ThisPS->GetPlayerName() == "P2" ||
			ThisGS->ReturnCurrentPlayerTurn() == 3 && ThisPS->GetPlayerName() == "P3" ||
			ThisGS->ReturnCurrentPlayerTurn() == 4 && ThisPS->GetPlayerName() == "P4")
		{
			ThisPS->SetPlayerLane(LaneDesintation);
		}
	}
}

//switch on/off this player controller's ability to interact with objects onscreen
void ASPlayerController::SwitchInteractability_Implementation(bool OnOrOFf)
{
	bEnableClickEvents = OnOrOFf;
	bEnableMouseOverEvents = OnOrOFf;
	bShowMouseCursor = OnOrOFf;
}

//replicate the score from the bet at a gambler space
void ASPlayerController::UpdateScoreFromGamblerSpace_Implementation(int AmountToUpdate)
{
	ASBoardGS* ThisGS = GetWorld()->GetGameState<ASBoardGS>();

	for (int i = 0; i < ThisGS->PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(ThisGS->PlayerArray[i]);

		if (ThisGS->ReturnCurrentPlayerTurn() == 1 && ThisPS->GetPlayerName() == "P1" ||
			ThisGS->ReturnCurrentPlayerTurn() == 2 && ThisPS->GetPlayerName() == "P2" ||
			ThisGS->ReturnCurrentPlayerTurn() == 3 && ThisPS->GetPlayerName() == "P3" ||
			ThisGS->ReturnCurrentPlayerTurn() == 4 && ThisPS->GetPlayerName() == "P4")
		{
			ThisPS->UpdatePlayerScore(AmountToUpdate);
		}
	}
}

//update widget text with text/color of duel choices
void ASPlayerController::AddPlayersToDuelWidget_Implementation(const FString& B1, FLinearColor C1, const FString& B2, FLinearColor C2, const FString& B3, FLinearColor C3)
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	UWidgetManager* TheManager = Cast<UWidgetManager>(FoundWidgets[0]);

	TheManager->SwitchWidgets(6);

	DuelWidget = Cast<USDuelChoice>(TheManager->SwitchReturn());

	DuelWidget->SetDuelOptions(B1, C1, B2, C2, B3, C3);
}

//The following methods are used to call Server functions from client computers
void ASPlayerController::SunWidgetCompleted_Implementation()
{ GetWorld()->GetGameState<ASBoardGS>()->NextOnSunWidget(); }

void ASPlayerController::UpdatePlayerSpaceNumbers_Implementation(APawn* PawnToMove, int LaneToMoveTo, int CurrentLane)
{ GetWorld()->GetGameState<ASBoardGS>()->WormHoleMovement(PawnToMove, LaneToMoveTo, CurrentLane); }

void ASPlayerController::UpdateLaneChoiceOnGS_Implementation()
{ GetWorld()->GetGameState<ASBoardGS>()->LaneChoice(true); }

void ASPlayerController::PassMouseInputToGS_Implementation(int CardIndex, const FString &BoolToPass, bool WhatStatus)
{ GetWorld()->GetGameState<ASBoardGS>()->ReplicateBoolToCards(CardIndex, BoolToPass, WhatStatus); }

void ASPlayerController::SunScalePassToGS_Implementation()
{ GetWorld()->GetGameState<ASBoardGS>()->UpdateNovaSun(); }

void ASPlayerController::TOCardChosen_Implementation(const FString& ChosenCard)
{ GetWorld()->GetGameState<ASBoardGS>()->TOCardChosen(ChosenCard, GetPlayerState<ASPlayerState>()->GetPlayerName()); }

void ASPlayerController::CallDuelUpdate_Implementation()
{ GetWorld()->GetGameState<ASBoardGS>()->UpdateDuelWidget(); }

void ASPlayerController::DuelChoicesPassToGS_Implementation(const FString& Choice1, const FString& Choice2)
{ GetWorld()->GetGameState<ASBoardGS>()->UpdateDuelChoices(Choice1, Choice2); }

void ASPlayerController::SetPlayerToDumpSpace_Implementation()
{ GetWorld()->GetGameState<ASBoardGS>()->SetPawnAtDump(); }