// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/Duel/SDuelGM.h"

//ENGINE HEADERS
#include "Kismet/GameplayStatics.h"

//CLASS HEADERS
#include "Minigames/Battle/SCharacter.h"
#include "Minigames/Battle/SWeapon.h"
#include "SPlayerController.h"
#include "SPlayerState.h"

void ASDuelGM::BeginPlay()
{
	Super::BeginPlay();

	//after a delay, assign the involved Splayers to their pawns
	FTimerHandle THandle_AssignDuelPlayers;
	GetWorldTimerManager().SetTimer(THandle_AssignDuelPlayers, this, &ASDuelGM::AssignPlayersToPawn, 3.0f, false);
}

void ASDuelGM::AssignPlayersToPawn()
{
	//bools to determine which pawn and camera to assign
	bool Player1Set = false;
	bool Player1CamSet = false;
	
	//access all instances of character pawns and weaponsS
	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASCharacter::StaticClass(), FoundCharacters);
	TArray<AActor*> FoundWeapons;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASWeapon::StaticClass(), FoundWeapons);

	//assign each involved player to a pawn/weapon and destroy the others
	if (FoundCharacters.Num() > 1)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* ThisPC = It->Get();

			if (ThisPC)
			{
				ASPlayerState* ThisPS = ThisPC->GetPlayerState<ASPlayerState>();

				if (ThisPS)
				{
					ASCharacter* ThisCharacter;

					//if the player is involved in the duel, give them control of an unused pawn
					if (ThisPS->ReturnIsInDuel())
					{
						if (!Player1Set)
						{
							ThisCharacter = Cast<ASCharacter>(FoundCharacters[0]);
							Player1Set = true;
						}
						else
						{ ThisCharacter = Cast<ASCharacter>(FoundCharacters[1]); }

						ThisPC->Possess(ThisCharacter);
						ThisPC->SetViewTarget(ThisCharacter);
						ThisCharacter->SetMinigamePawnColor(ThisPS->ReturnPlayerColor());
					}
					//if the player is not involved in the duel, allow it to attach to the camera of one of the dueling players
					else
					{
						ThisPC->UnPossess();

						if (!Player1CamSet)
						{
							ThisCharacter = Cast<ASCharacter>(FoundCharacters[0]);
							Player1CamSet = true;
						}
						else
						{ ThisCharacter = Cast<ASCharacter>(FoundCharacters[1]); }

						ThisPC->SetViewTarget(ThisCharacter);
					}
				}
			}
		}

		//destroy the unused pawns and weapons
		for (int i = 2; i < FoundCharacters.Num(); ++i)
		{
			FoundCharacters[i]->Destroy();
			FoundWeapons[i]->Destroy();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(4212, 20.0f, FColor::Yellow, "Not Enough Characters Found");
	}	
}

//call seamless travel back to the board level
void ASDuelGM::BackToBoard()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Levels/MapTest?listen");
}