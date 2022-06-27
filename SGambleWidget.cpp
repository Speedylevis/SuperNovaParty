// Fill out your copyright notice in the Description page of Project Settings.


#include "SGambleWidget.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SPlayerState.h"

//COMPONENT HEADERS
#include "Components/Button.h"
#include "Components/TextBlock.h"

bool USGambleWidget::Initialize()
{
	Super::Initialize();

	//binds dynamic delegates
	OnePod_Button->OnClicked.AddDynamic(this, &USGambleWidget::BetOnePod);
	TwoPod_Button->OnClicked.AddDynamic(this, &USGambleWidget::BetTwoPod);
	ThreePod_Button->OnClicked.AddDynamic(this, &USGambleWidget::BetThreePod);

	return true;
}

//set bet based on button 1 being pressed
void USGambleWidget::BetOnePod()
{
	Bet = 1;
	Gamble(Bet);
}

//set bet based on button 2 being pressed
void USGambleWidget::BetTwoPod()
{
	Bet = 2;
	Gamble(Bet);
}

//set bet based on button 3 being pressed
void USGambleWidget::BetThreePod()
{
	Bet = 3;
	Gamble(Bet);
}

void USGambleWidget::Gamble(int8 ThisBet)
{
	//flip of coin to decide if bet wins (1) or loses (0)
	int8 Flip = FMath::RandRange(0, 1);
	int UpdateAmount;

	if(Flip == 0)
	{ UpdateAmount = -ThisBet; }
	else
	{ UpdateAmount = ThisBet * 2; }

	ASPlayerController* ThisPC = GetWorld()->GetFirstPlayerController<ASPlayerController>();
	ThisPC->UpdateScoreFromGamblerSpace(UpdateAmount);
	ThisPC->WidgetCompleted();
}