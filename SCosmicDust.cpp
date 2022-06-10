// Fill out your copyright notice in the Description page of Project Settings.


#include "SCosmicDust.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlanet.h"

ASCosmicDust::ASCosmicDust()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASCosmicDust::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		GEngine->AddOnScreenDebugMessage(1822, 5.f, FColor::Turquoise, "Landed on Cosmic Dust");
		IncrementPodCount();
	}
}

//increase the PodBank value on the connected planet
void ASCosmicDust::IncrementPodCount()
{
	if (ConnectedPlanet)
	{
		int AmountToIncrement = 2;
		//If a player has the red card activated, double the effects of this space
		if(GetWorld()->GetGameState<ASBoardGS>()->ReturnRedCardActive())
		{ AmountToIncrement = 4; }

		ConnectedPlanet->SetPodBank(AmountToIncrement);
	}

	bIsActive = false;
	NextTurn();
}
