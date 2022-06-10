// Fill out your copyright notice in the Description page of Project Settings.


#include "SWormholeWidget.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SPlayerState.h"

//COMPONENT HEADERS
#include"Components/Button.h"
#include"Components/TextBlock.h"

bool USWormholeWidget::Initialize()
{
	Super::Initialize();

	//bind buttons to functions
	LaneChoice_1->OnClicked.AddDynamic(this, &USWormholeWidget::Wormhole_L1);
	LaneChoice_2->OnClicked.AddDynamic(this, &USWormholeWidget::Wormhole_L2);
	LaneChoice_3->OnClicked.AddDynamic(this, &USWormholeWidget::Wormhole_L3);
	LaneChoice_4->OnClicked.AddDynamic(this, &USWormholeWidget::Wormhole_L4);
	LaneChoice_5->OnClicked.AddDynamic(this, &USWormholeWidget::Wormhole_L5);

	//set initial text as empty
	PlayerNames_L1->SetText(FText::FromString(""));
	PlayerNames_L2->SetText(FText::FromString(""));
	PlayerNames_L3->SetText(FText::FromString(""));
	PlayerNames_L4->SetText(FText::FromString(""));
	PlayerNames_L5->SetText(FText::FromString(""));

	PlanetScore_L1->SetText(FText::FromString(""));
	PlanetScore_L2->SetText(FText::FromString(""));
	PlanetScore_L3->SetText(FText::FromString(""));
	PlanetScore_L4->SetText(FText::FromString(""));
	PlanetScore_L5->SetText(FText::FromString(""));

	return true;
}

//pass the lane choice based on the button chosen
void USWormholeWidget::Wormhole_L1()
{
	WormholeChosen(1);
}

void USWormholeWidget::Wormhole_L2()
{
	WormholeChosen(2);
}

void USWormholeWidget::Wormhole_L3()
{
	WormholeChosen(3);
}

void USWormholeWidget::Wormhole_L4()
{
	WormholeChosen(4);
}

void USWormholeWidget::Wormhole_L5()
{
	WormholeChosen(5);
}

//call a server function to move the player and have it replicate
void USWormholeWidget::WormholeChosen(int WormholeChoice)
{
	ASBoardGS* ThisGS = GetWorld()->GetGameState<ASBoardGS>();
	ASPlayerController* ThisPC = GetWorld()->GetFirstPlayerController<ASPlayerController>();
	ASPlayerState* ThisPS = ThisPC->GetPlayerState<ASPlayerState>();

	ThisPC->UpdatePlayerSpaceNumbers(ThisPS->ReturnPlayerPawn(), WormholeChoice, ThisPS->ReturnPlayerLane());
	ThisPC->WidgetCompleted();
}

//set the widget to show where players are and the values of each planet
void USWormholeWidget::SetWormholeText()
{
	LaneChoice_1->SetIsEnabled(true);
	LaneChoice_1->SetIsEnabled(true);
	LaneChoice_1->SetIsEnabled(true);
	LaneChoice_1->SetIsEnabled(true);
	LaneChoice_1->SetIsEnabled(true);

	//reset text each time widget is brought up
	PlayerNames_L1->SetText(FText::FromString(""));
	PlayerNames_L2->SetText(FText::FromString(""));
	PlayerNames_L3->SetText(FText::FromString(""));
	PlayerNames_L4->SetText(FText::FromString(""));
	PlayerNames_L5->SetText(FText::FromString(""));

	PlanetScore_L1->SetText(FText::FromString(""));
	PlanetScore_L2->SetText(FText::FromString(""));
	PlanetScore_L3->SetText(FText::FromString(""));
	PlanetScore_L4->SetText(FText::FromString(""));
	PlanetScore_L5->SetText(FText::FromString(""));

	ASBoardGS* ThisGS = GetWorld()->GetGameState<ASBoardGS>();
	ASPlayerController* ThisPC = GetWorld()->GetFirstPlayerController<ASPlayerController>();
	ASPlayerState* ThisPS = ThisPC->GetPlayerState<ASPlayerState>();
	FString TempString = "";

	//allow current player to interact with widget
	if ((ThisGS->ReturnCurrentPlayerTurn() == 1 && ThisPS->GetPlayerName() == "P1") ||
		(ThisGS->ReturnCurrentPlayerTurn() == 2 && ThisPS->GetPlayerName() == "P2") ||
		(ThisGS->ReturnCurrentPlayerTurn() == 3 && ThisPS->GetPlayerName() == "P3") ||
		(ThisGS->ReturnCurrentPlayerTurn() == 4 && ThisPS->GetPlayerName() == "P4"))
	{
		ThisPC->SwitchInteractability(true);
	}

	//add each player to the lane they are currently in
	for (int i = 0; i < ThisGS->PlayerArray.Num(); ++i)
	{
		ThisPS = Cast<ASPlayerState>(ThisGS->PlayerArray[i]);
		
		TempString = "";

		if (ThisPS->ReturnPlayerLane() == 1)
		{
			TempString = PlayerNames_L1->GetText().ToString();

			if (TempString == "")
			{ TempString.Append(ThisPS->GetPlayerName()); }
			else
			{ TempString.Append(", " + ThisPS->GetPlayerName()); }

			PlayerNames_L1->SetText(FText::FromString(TempString));
		}
		else if (ThisPS->ReturnPlayerLane() == 2)
		{
			TempString = PlayerNames_L2->GetText().ToString();

			if (TempString == "")
			{ TempString.Append(ThisPS->GetPlayerName()); }
			else
			{ TempString.Append(", " + ThisPS->GetPlayerName()); }

			PlayerNames_L2->SetText(FText::FromString(TempString));
		}
		else if (ThisPS->ReturnPlayerLane() == 3)
		{
			TempString = PlayerNames_L3->GetText().ToString();

			if (TempString == "")
			{ TempString.Append(ThisPS->GetPlayerName()); }
			else
			{ TempString.Append(", " + ThisPS->GetPlayerName()); }

			PlayerNames_L3->SetText(FText::FromString(TempString));
		}
		else if (ThisPS->ReturnPlayerLane() == 4)
		{
			TempString = PlayerNames_L4->GetText().ToString();

			if (TempString == "")
			{ TempString.Append(ThisPS->GetPlayerName()); }
			else
			{ TempString.Append(", " + ThisPS->GetPlayerName()); }

			PlayerNames_L4->SetText(FText::FromString(TempString));
		}
		else if (ThisPS->ReturnPlayerLane() == 5)
		{
			TempString = PlayerNames_L5->GetText().ToString();

			if (TempString == "")
			{ TempString.Append(ThisPS->GetPlayerName()); }
			else
			{ TempString.Append(", " + ThisPS->GetPlayerName()); }

			PlayerNames_L5->SetText(FText::FromString(TempString));
		}
	}

	PlanetBanks = ThisGS->ReturnPlanetScores();

	//add the value of each planet to the widget
	if (PlanetBanks.Num() > 0)
	{
		for (int i = 0; i < PlanetBanks.Num(); i++)
		{
			if (i == 0)
			{ PlanetScore_L1->SetText(FText::AsNumber(PlanetBanks[i])); }
			else if (i == 1)
			{ PlanetScore_L2->SetText(FText::AsNumber(PlanetBanks[i])); }
			else if (i == 2)
			{ PlanetScore_L3->SetText(FText::AsNumber(PlanetBanks[i])); }
			else if (i == 3)
			{ PlanetScore_L4->SetText(FText::AsNumber(PlanetBanks[i])); }
			else if (i == 4)
			{ PlanetScore_L5->SetText(FText::AsNumber(PlanetBanks[i])); }
		}
	}
}

//deactivate the button of the wormhole the current player is on
void USWormholeWidget::DeactivateButton(int ButtonToDeactivate)
{
	if (ButtonToDeactivate == 1)
	{ LaneChoice_1->SetIsEnabled(false); }
	else if (ButtonToDeactivate == 2)
	{ LaneChoice_2->SetIsEnabled(false); }
	else if (ButtonToDeactivate == 3)
	{ LaneChoice_3->SetIsEnabled(false); }
	else if (ButtonToDeactivate == 4)
	{ LaneChoice_4->SetIsEnabled(false); }
	else if (ButtonToDeactivate == 5)
	{ LaneChoice_5->SetIsEnabled(false); }
}