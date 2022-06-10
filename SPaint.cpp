// Fill out your copyright notice in the Description page of Project Settings.

#include "SPaint.h"

// CLASS HEADERS
#include "SBoardGS.h"
#include "SCardSystem.h"
#include "SPlayerState.h"


ASPaint::ASPaint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASPaint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		PaintCard();
	}
}

//assign a random color/effect to a random card of the player that lands on this space
void ASPaint::PaintCard()
{
	if (GetWorld()->IsServer())
	{
		int RandomCard = FMath::RandRange(0, 2);

		int RandomInt = FMath::RandRange(0, 2);

		FString RandomColor;

		if (RandomInt == 0)
		{ RandomColor = "Red"; }
		else if (RandomInt == 1)
		{ RandomColor = "Blue"; }
		else if (RandomInt == 2)
		{ RandomColor = "Yellow"; }

		ASBoardGS* ThisGS = Cast<ASBoardGS>(GetWorld()->GetGameState());

		//match the player name to the current player turn and assign the random color/effect to the random card
		for (int i = 0; i < ThisGS->PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(ThisGS->PlayerArray[i]);

			if (ThisGS->ReturnCurrentPlayerTurn() == 1 && ThisPS->GetPlayerName() == "P1" ||
				ThisGS->ReturnCurrentPlayerTurn() == 2 && ThisPS->GetPlayerName() == "P2" ||
				ThisGS->ReturnCurrentPlayerTurn() == 3 && ThisPS->GetPlayerName() == "P3" ||
				ThisGS->ReturnCurrentPlayerTurn() == 4 && ThisPS->GetPlayerName() == "P4")
			{
				ThisPS->SetPlayerPaints(RandomCard, RandomColor);
			}
		}

		bIsActive = false;
		NextTurn();
	}
}