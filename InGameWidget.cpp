// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"

//COMPONENT HEADERS
#include "Components/TextBlock.h"

//update the text for the current player's turn and the current turn number
void UInGameWidget::SetPlayerTurn(FString PTurn)
{ PlayerTurn->SetText(FText::FromString(PTurn)); }
void UInGameWidget::SetTurnNumber(FString NTurn)
{ TurnNumber->SetText(FText::FromString(NTurn)); }