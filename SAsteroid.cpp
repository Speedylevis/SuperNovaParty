// Fill out your copyright notice in the Description page of Project Settings.


#include "SAsteroid.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlanet.h"

ASAsteroid::ASAsteroid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsActive)
	{
		GEngine->AddOnScreenDebugMessage(1821, 5.f, FColor::Turquoise, "Landed on Asteroid");
		DecrementPodCount();
	}
}

//lower the PodBank value on the connected planet
void ASAsteroid::DecrementPodCount()
{
	if (ConnectedPlanet)
	{
		int AmountToDecrement = -2;
		//If a player has the red card activated, double the effects of this space
		if(GetWorld()->GetGameState<ASBoardGS>()->ReturnRedCardActive())
		{ AmountToDecrement = -4; }

		ConnectedPlanet->SetPodBank(AmountToDecrement);
	}

	bIsActive = false;
	NextTurn();
}