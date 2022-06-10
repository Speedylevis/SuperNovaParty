// Fill out your copyright notice in the Description page of Project Settings.


#include "SWinWidget.h"

//COMPONENT HEADERS
#include "Components/TextBlock.h"

//update the text and its color to reflect the winner of the game
void USWinWidget::UpdateWinningText(FString WinnerName, FLinearColor WinnerColor)
{
	if (WinnerName == "P1")
	{ WinningPlayer->SetText(FText::FromString("P1 Wins!")); }
	else if (WinnerName == "P2")
	{ WinningPlayer->SetText(FText::FromString("P2 Wins!")); }
	else if (WinnerName == "P3")
	{ WinningPlayer->SetText(FText::FromString("P3 Wins!")); }
	else if (WinnerName == "P4")
	{ WinningPlayer->SetText(FText::FromString("P4 Wins!")); }

	if (WinnerColor == FLinearColor::Blue)
	{ WinningPlayer->SetColorAndOpacity(FLinearColor::Blue); }
	if (WinnerColor == FLinearColor::Green)
	{ WinningPlayer->SetColorAndOpacity(FLinearColor::Green); }
	if (WinnerColor == FLinearColor::Red)
	{ WinningPlayer->SetColorAndOpacity(FLinearColor::Red); }
	if (WinnerColor == FLinearColor::Yellow)
	{ WinningPlayer->SetColorAndOpacity(FLinearColor::Yellow); }
	
}