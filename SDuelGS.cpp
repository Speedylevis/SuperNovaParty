// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/Duel/SDuelGS.h"

//CLASS HEADERS
#include "Minigames/Duel/SDuelGM.h"
#include "SPlayerState.h"

//assign the duel winner and call travel back to the board
void ASDuelGS::DuelOver_Implementation(const FString& LosingPlayer)
{
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);
		
		if (ThisPS->ReturnIsInDuel() && ThisPS->GetPlayerName() != LosingPlayer)
		{
			ThisPS->SetIsDuelWinner(true);
		}
		
		ThisPS->SetIsInDuel(false);
	}

	ASDuelGM* ThisGM = GetWorld()->GetAuthGameMode<ASDuelGM>();
	FTimerHandle THandle_BTBTimer;
	GetWorldTimerManager().SetTimer(THandle_BTBTimer, ThisGM, &ASDuelGM::BackToBoard, 3.0f, false);
}