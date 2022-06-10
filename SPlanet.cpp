// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlanet.h"

//ENGINE HEADERS
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SPlayerPawn.h"
#include "SPlayerState.h"
#include "SSunSpace.h"


// Sets default values
ASPlanet::ASPlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASPlanet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlanet, PodBank);
}

// Called every frame
void ASPlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		GEngine->AddOnScreenDebugMessage(1821, 5.f, FColor::Turquoise, "Landed on Planet: " + FString::FromInt(PodBank) + " Pods");
		DepositPodBank();
		bIsActive = false;
	}
}

//update PodBank on server to have it replicate
void ASPlanet::SetPodBank_Implementation(int PodUpdate)
{
	PodBank += PodUpdate;
}

//update the score of the player that landed on this planet and reset the PodBank
void ASPlanet::DepositPodBank_Implementation()
{	
	int CurrentTurn = GetWorld()->GetGameState<ASBoardGS>()->ReturnCurrentPlayerTurn();

	OrbitingPlayer->GetPlayerState<ASPlayerState>()->UpdatePlayerScore(PodBank);
	
	PodBank = 10;

	ASPlayerPawn* ThisPawn = Cast<ASPlayerPawn>(OrbitingPlayer);

	//move the player back to the sun space so they can pick their new lane
	if (SunSpace)
	{
		if (SunSpace->ReturnPawnCount() == 0)
		{
			OrbitingPlayer->SetActorLocation(SunSpace->GetTransitionPointA());
			SunSpace->SetPawnCount(1);
		}
		else if (SunSpace->ReturnPawnCount() == 1)
		{
			OrbitingPlayer->SetActorLocation(SunSpace->GetTransitionPointB());
			SunSpace->SetPawnCount(2);
		}
		else if (SunSpace->ReturnPawnCount() == 2)
		{
			OrbitingPlayer->SetActorLocation(SunSpace->GetTransitionPointC());
			SunSpace->SetPawnCount(3);
		}
		else if (SunSpace->ReturnPawnCount() == 3)
		{
			OrbitingPlayer->SetActorLocation(SunSpace->GetTransitionPointD());
			SunSpace->SetPawnCount(4);
		}

		if (ThisPawn)
		{
			ThisPawn->SetCurrentSpace(SunSpace);
			SunSpace->SetOrbitingPlayer(OrbitingPlayer);
			OrbitingPlayer = nullptr;
		}
	}

	//activate the SunWidget and set interactability for the player to pick their new lane
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		ThisPC->ChangeWidgets(3);
		FString ThisName = ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName();

		if (CurrentTurn == 1 && ThisName == "P1" ||
			CurrentTurn == 2 && ThisName == "P2" ||
			CurrentTurn == 3 && ThisName == "P3" ||
			CurrentTurn == 4 && ThisName == "P4")
		{
			ThisPC->SwitchInteractability(true);
		}
	}
}