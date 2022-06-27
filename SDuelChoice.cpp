// Fill out your copyright notice in the Description page of Project Settings.


#include "SDuelChoice.h"

//CLASS HEADERS
#include "SPlayerController.h"
#include "SPlayerState.h"

//COMPONENT HEADERS
#include "Components/Button.h"
#include "Components/TextBlock.h"

bool USDuelChoice::Initialize()
{
	Super::Initialize();

	//binds dynamic delegates
	DCButton_1->OnClicked.AddDynamic(this, &USDuelChoice::Button_1_Chosen);
	DCButton_2->OnClicked.AddDynamic(this, &USDuelChoice::Button_2_Chosen);
	DCButton_3->OnClicked.AddDynamic(this, &USDuelChoice::Button_3_Chosen);

	return true;
}

//pass the string of the player name tied to the chosen button
void USDuelChoice::Button_1_Chosen()
{
	ASPlayerController* ThisPC = GetWorld()->GetFirstPlayerController<ASPlayerController>();
	ASPlayerState* ThisPS = ThisPC->GetPlayerState<ASPlayerState>();

	ThisPC->DuelChoicesPassToGS(ThisPS->GetPlayerName(), DCText_1->GetText().ToString());
}
void USDuelChoice::Button_2_Chosen()
{
	ASPlayerController* ThisPC = GetWorld()->GetFirstPlayerController<ASPlayerController>();
	ASPlayerState* ThisPS = ThisPC->GetPlayerState<ASPlayerState>();

	ThisPC->DuelChoicesPassToGS(ThisPS->GetPlayerName(), DCText_2->GetText().ToString());
}
void USDuelChoice::Button_3_Chosen()
{
	ASPlayerController* ThisPC = GetWorld()->GetFirstPlayerController<ASPlayerController>();
	ASPlayerState* ThisPS = ThisPC->GetPlayerState<ASPlayerState>();

	ThisPC->DuelChoicesPassToGS(ThisPS->GetPlayerName(), DCText_3->GetText().ToString());
}

//update the text/color of the player choices
void USDuelChoice::SetDuelOptions(FString B1, FLinearColor C1, FString B2, FLinearColor C2, FString B3, FLinearColor C3)
{
	DCText_1->SetText(FText::FromString(B1));
	DCText_1->SetColorAndOpacity(C1);
	DCText_2->SetText(FText::FromString(B2));
	DCText_2->SetColorAndOpacity(C2);
	DCText_3->SetText(FText::FromString(B3));
	DCText_3->SetColorAndOpacity(C3);
}