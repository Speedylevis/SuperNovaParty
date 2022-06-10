// Fill out your copyright notice in the Description page of Project Settings.

#include "SSunWidget.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SPlayerState.h"

//ENGINE HEADERS
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

//COMPONENTS HEADERS
#include "Components/Button.h"
#include "Components/TextBlock.h"

bool USSunWidget::Initialize()
{
	Super::Initialize();

	//binds dynamic delegates
	LaneOne_Button->OnClicked.AddDynamic(this, &USSunWidget::SelectLaneOne);
	LaneTwo_Button->OnClicked.AddDynamic(this, &USSunWidget::SelectLaneTwo);
	LaneThree_Button->OnClicked.AddDynamic(this, &USSunWidget::SelectLaneThree);
	LaneFour_Button->OnClicked.AddDynamic(this, &USSunWidget::SelectLaneFour);
	LaneFive_Button->OnClicked.AddDynamic(this, &USSunWidget::SelectLaneFive);

	PlanetBanks.SetNum(5);

	return true;
}

//set the lane based on the button pressed, then initiate the player's movement to that lane
void USSunWidget::SelectLaneOne()
{
	Lane = 1;
	MoveToLane(Lane);
}

void USSunWidget::SelectLaneTwo()
{
	Lane = 2;
	MoveToLane(Lane);
}

void USSunWidget::SelectLaneThree()
{
	Lane = 3;
	MoveToLane(Lane);
}

void USSunWidget::SelectLaneFour()
{
	Lane = 4;
	MoveToLane(Lane);
}

void USSunWidget::SelectLaneFive()
{
	Lane = 5;
	MoveToLane(Lane);
}

//call player movement on server to have it replicate
void USSunWidget::MoveToLane(int8 LaneSelect)
{
	ASBoardGS* ThisGS = Cast<ASBoardGS>(GetWorld()->GetGameState());

	ASPlayerController* ThisPC = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	//match the player name to the current player turn and move that ship based on the lane chosen
	for (int i = 0; i < ThisGS->PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(ThisGS->PlayerArray[i]);
	
		if (ThisGS->ReturnCurrentPlayerTurn() == 1)
		{
			if (ThisPS->GetPlayerName() == "P1")
			{
				ThisPC->MovePlayerReplicate(ThisPS->ReturnPlayerPawn(), LaneSelect);
			}
		}
		else if (ThisGS->ReturnCurrentPlayerTurn() == 2)
		{
			if (ThisPS->GetPlayerName() == "P2")
			{
				ThisPC->MovePlayerReplicate(ThisPS->ReturnPlayerPawn(), LaneSelect);
			}
		}
		else if (ThisGS->ReturnCurrentPlayerTurn() == 3)
		{
			if (ThisPS->GetPlayerName() == "P3")
			{
				ThisPC->MovePlayerReplicate(ThisPS->ReturnPlayerPawn(), LaneSelect);
			}
		}
		else if (ThisGS->ReturnCurrentPlayerTurn() == 4)
		{
			if (ThisPS->GetPlayerName() == "P4")
			{
				ThisPC->MovePlayerReplicate(ThisPS->ReturnPlayerPawn(), LaneSelect);
			}
		}
	}

	ThisPC->UpdateLaneChoiceOnGS();
}

//set the text on the widget to show what lanes each player is in
void USSunWidget::SetPlayersText()
{
	LaneOne_Players_Text->SetText(FText::FromString("Players:"));
	LaneTwo_Players_Text->SetText(FText::FromString("Players:"));
	LaneThree_Players_Text->SetText(FText::FromString("Players:"));
	LaneFour_Players_Text->SetText(FText::FromString("Players:"));
	LaneFive_Players_Text->SetText(FText::FromString("Players:"));

	LaneOne_Pod_Text->SetText(FText::FromString("Available Pods:"));
	LaneTwo_Pod_Text->SetText(FText::FromString("Available Pods:"));
	LaneThree_Pod_Text->SetText(FText::FromString("Available Pods:"));
	LaneFour_Pod_Text->SetText(FText::FromString("Available Pods:"));
	LaneFive_Pod_Text->SetText(FText::FromString("Available Pods:"));

	ASBoardGS* ThisGS = Cast<ASBoardGS>(GetWorld()->GetGameState());

	if (ThisGS)
	{
		for (int i = 0; i < ThisGS->PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(ThisGS->PlayerArray[i]);

			if (ThisPS->ReturnPlayerLane() != 0)
			{
				if (ThisPS->ReturnPlayerLane() == 1)
				{
					FString TempString;

					TempString = LaneOne_Players_Text->GetText().ToString();

					if (!TempString.Contains(ThisPS->GetPlayerName()))
					{
						TempString.Append("\n" + ThisPS->GetPlayerName());

						LaneOne_Players_Text->SetText(FText::FromString(TempString));
					}
				}
				else if (ThisPS->ReturnPlayerLane() == 2)
				{
					FString TempString;

					TempString = LaneTwo_Players_Text->GetText().ToString();

					if (!TempString.Contains(ThisPS->GetPlayerName()))
					{
						TempString.Append("\n" + ThisPS->GetPlayerName());

						LaneTwo_Players_Text->SetText(FText::FromString(TempString));
					}
				}
				else if (ThisPS->ReturnPlayerLane() == 3)
				{
					FString TempString;

					TempString = LaneThree_Players_Text->GetText().ToString();

					if (!TempString.Contains(ThisPS->GetPlayerName()))
					{
						TempString.Append("\n" + ThisPS->GetPlayerName());

						LaneThree_Players_Text->SetText(FText::FromString(TempString));
					}
				}
				else if (ThisPS->ReturnPlayerLane() == 4)
				{
					FString TempString;

					TempString = LaneFour_Players_Text->GetText().ToString();

					if (!TempString.Contains(ThisPS->GetPlayerName()))
					{
						TempString.Append("\n" + ThisPS->GetPlayerName());

						LaneFour_Players_Text->SetText(FText::FromString(TempString));
					}
				}
				else if (ThisPS->ReturnPlayerLane() == 5)
				{
					FString TempString;

					TempString = LaneFive_Players_Text->GetText().ToString();

					if (!TempString.Contains(ThisPS->GetPlayerName()))
					{
						TempString.Append("\n" + ThisPS->GetPlayerName());

						LaneFive_Players_Text->SetText(FText::FromString(TempString));
					}
				}
			}
		}
	}
}

//set the text on the widget to show the pod value each planet has
void USSunWidget::SetPlanetsText()
{
	ASBoardGS* ThisGS = Cast<ASBoardGS>(GetWorld()->GetGameState());

	PlanetBanks = ThisGS->ReturnPlanetScores();

	if (PlanetBanks.Num() > 0)
	{
		for (int i = 0; i < PlanetBanks.Num(); i++)
		{
			FString TempString;

			if (i == 0)
			{
				TempString = LaneOne_Pod_Text->GetText().ToString();

				TempString.Append("\n" + FString::FromInt(PlanetBanks[i]));

				LaneOne_Pod_Text->SetText(FText::FromString(TempString));
			}
			else if (i == 1)
			{
				TempString = LaneTwo_Pod_Text->GetText().ToString();

				TempString.Append("\n" + FString::FromInt(PlanetBanks[i]));

				LaneTwo_Pod_Text->SetText(FText::FromString(TempString));
			}
			else if (i == 2)
			{
				TempString = LaneThree_Pod_Text->GetText().ToString();

				TempString.Append("\n" + FString::FromInt(PlanetBanks[i]));

				LaneThree_Pod_Text->SetText(FText::FromString(TempString));
			}
			else if (i == 3)
			{
				TempString = LaneFour_Pod_Text->GetText().ToString();

				TempString.Append("\n" + FString::FromInt(PlanetBanks[i]));

				LaneFour_Pod_Text->SetText(FText::FromString(TempString));
			}
			else if (i == 4)
			{
				TempString = LaneFive_Pod_Text->GetText().ToString();

				TempString.Append("\n" + FString::FromInt(PlanetBanks[i]));

				LaneFive_Pod_Text->SetText(FText::FromString(TempString));
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(4812, 20.0f, FColor::Red, "No Planets");
	}
}