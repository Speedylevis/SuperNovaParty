// Fill out your copyright notice in the Description page of Project Settings.


#include "SBoardGS.h"

//ENGINE HEADERS
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

//CLASS HEADERS
#include "BoardGM.h"
#include "SCardSystem.h"
#include "SFreeCamera.h"
#include "SPlanet.h"
#include "SPlayerController.h"
#include "SPlayerPawn.h"
#include "SPlayerState.h"
#include "SSun.h"
#include "SSunSpace.h"
#include "STOCards.h"
#include "SWormhole.h"

//constructor to set the size of the arrays
ASBoardGS::ASBoardGS(const FObjectInitializer& ObjectInitializer)
{
	//set the number of items in each array
	TurnOrderArray.SetNum(4);
	PlayerPawns.SetNum(4);
	CurrentCards.SetNum(3);
	PlayerTurnOrder.SetNum(4);
	TOCards.SetNum(4);
	ColorArray.SetNum(4);
	PlayerScoreArray.SetNum(4);

	PlanetCounts.SetNum(5);

	//set to allow for variable replication
	bAlwaysRelevant = true;
	bReplicates = true;
}

//replicated variables need to be added to DOREPLIFETIME
void ASBoardGS::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASBoardGS, PlayerTurnOrder);
	DOREPLIFETIME(ASBoardGS, TOCardCounter);
	DOREPLIFETIME(ASBoardGS, ColorArray);
	DOREPLIFETIME(ASBoardGS, PlayerPawns);
	DOREPLIFETIME(ASBoardGS, CurrentPlayerTurn);
	DOREPLIFETIME(ASBoardGS, PlayerScoreArray);
	DOREPLIFETIME(ASBoardGS, TurnCount);
	DOREPLIFETIME(ASBoardGS, ReturningFromMinigame);
	DOREPLIFETIME(ASBoardGS, IsOnWormhole);
}

//setup for allowing players to pick the turn order
void ASBoardGS::InitialTurnOrder()
{
	ReturningFromMinigame = false;

	//if any of the players have a score, then the game has already been initialized and the players are returning from a level transition
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

		if (ThisPS->ReturnPlayerScore() != 0)
		{
			ReturningFromMinigame = true;
			break;
		}
	}

	//if returning from a level transition, ignore InitialSetup and move to PostMinigameSetup
	if (ReturningFromMinigame)
	{
		FTimerHandle THandle_PostMinigameSetupCall;
		GetWorldTimerManager().SetTimer(THandle_PostMinigameSetupCall, this, &ASBoardGS::PostMinigameSetup, 2.0f, false);
		return;
	}

	//spawn the turn order camera pawn and store a reference to allow possession later on
	CameraPawn = GetWorld()->SpawnActor<APawn>(TOCameraPawnClass, FVector(-5000.0f, 3500.0f, 5500.0f), FRotator(0.0f, 0.0f, 0.0f));

	if (GetWorld()->IsServer())
	{
		//name each player state first to use in setting turn order
		for (int i = 0; i < PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

			if(i == 0)
			{ ThisPS->SetPlayerName("P1"); }
			else if(i == 1)
			{ ThisPS->SetPlayerName("P2"); }
			else if(i == 2)
			{ ThisPS->SetPlayerName("P3"); }
			else if(i == 3)
			{ ThisPS->SetPlayerName("P4"); }
		}

		//randomize the array of strings to set the turn order cards in a random order
		PlayerTurnOrder[0] = "1";
		PlayerTurnOrder[1] = "2";
		PlayerTurnOrder[2] = "3";
		PlayerTurnOrder[3] = "4";

		if (PlayerTurnOrder.Num() > 0)
		{
			int LastIndex = PlayerTurnOrder.Num() - 1;
			for (int i = 0; i <= LastIndex; ++i)
			{
				int Index = FMath::RandRange(i, LastIndex);
				if (i != Index)
				{
					PlayerTurnOrder.Swap(i, Index);
				}
			}
		}
	}

	//spawn the turn order cards and store references
	TOCards[0] = GetWorld()->SpawnActor<AActor>(CardBP, FVector(-4750.0f, 3350.0f, 5500.0f), FRotator(0.0f, 90.0f, 0.0f));
	TOCards[1] = GetWorld()->SpawnActor<AActor>(CardBP, FVector(-4750.0f, 3450.0f, 5500.0f), FRotator(0.0f, 90.0f, 0.0f));
	TOCards[2] = GetWorld()->SpawnActor<AActor>(CardBP, FVector(-4750.0f, 3550.0f, 5500.0f), FRotator(0.0f, 90.0f, 0.0f));
	TOCards[3] = GetWorld()->SpawnActor<AActor>(CardBP, FVector(-4750.0f, 3650.0f, 5500.0f), FRotator(0.0f, 90.0f, 0.0f));

	//set the value of the turn order cards based on the randomized array
	for (int i = 0; i < TOCards.Num(); ++i)
	{
		if (i == 0)
		{ Cast<ASTOCards>(TOCards[i])->SetCardValue(PlayerTurnOrder[0]); }
		else if (i == 1)
		{ Cast<ASTOCards>(TOCards[i])->SetCardValue(PlayerTurnOrder[1]); }
		else if (i == 2)
		{ Cast<ASTOCards>(TOCards[i])->SetCardValue(PlayerTurnOrder[2]); }
		else if (i == 3)
		{ Cast<ASTOCards>(TOCards[i])->SetCardValue(PlayerTurnOrder[3]); }

		Cast<ASTOCards>(TOCards[i])->SetupCard();
	}

	//have a delay before possession to allow clients to catch up to the spawning of the pawn
	FTimerHandle THandle_TOCameraPossess;
	GetWorldTimerManager().SetTimer(THandle_TOCameraPossess, this, &ASBoardGS::PossessTOCameraPawn, 0.2f, false);
}

//setup player controllers for turn order selection
void ASBoardGS::PossessTOCameraPawn()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		//have all server/clients possess the turn order pawn and allow for selection turn order cards
		ThisPC->Possess(CameraPawn);
		ThisPC->SwitchInteractability(true);
		ThisPC->SetAbleToBePaused(false);
	}
}

//activate turn order card choice on all server/client
void ASBoardGS::TOCardChosen_Implementation(const FString& ChosenCard, const FString& PName)
{
	//assign the player's turn order based on the card they choise
	for (int i = 1; i < 5; ++i)
	{
		if (FCString::Atoi(*ChosenCard) == i)
		{
			if (PName == "P1")
			{ TurnOrderArray[i - 1] = 1; }
			else if (PName == "P2")
			{ TurnOrderArray[i - 1] = 2; }
			else if (PName == "P3")
			{ TurnOrderArray[i - 1] = 3; }
			else if (PName == "P4")
			{ TurnOrderArray[i - 1] = 4; }
		}
	}

	TArray<AActor*> FoundCards;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTOCards::StaticClass(), FoundCards);

	//reveal the chosen card and disable the ability to interact with it
	for (int i = 0; i < FoundCards.Num(); ++i)
	{
		if (Cast<ASTOCards>(FoundCards[i])->ReturnCardValue() == ChosenCard)
		{
			FoundCards[i]->SetActorRotation(FRotator(0.0f, 270.0f, 0.0f));
			Cast<ASTOCards>(FoundCards[i])->DisableAbilityToClick();
		}
	}

	//increment the number of turn order cards chosen
	++TOCardCounter;

	//when all 4 cards are chosen, assign the first player's turn and move to the board setup
	if (TOCardCounter >= 4)
	{
		CurrentPlayerTurn = TurnOrderArray[0];

		FTimerHandle THandle_InitialSetupCall;
		GetWorldTimerManager().SetTimer(THandle_InitialSetupCall, this, &ASBoardGS::InitialSetup, 1.5f, false);
	}
}

//This method is to speed up testing by not requiring the players to go through TO setup
void ASBoardGS::SkipTurnOrderSetup()
{
	ReturningFromMinigame = false;

	//if any of the players have a score, then the game has already been initialized and the players are returning from a level transition
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

		if (ThisPS->ReturnPlayerScore() != 0)
		{
			ReturningFromMinigame = true;
			break;
		}
	}

	//if returning from a level transition, ignore InitialSetup and move to PostMinigameSetup
	if (ReturningFromMinigame)
	{
		FTimerHandle THandle_PostMinigameSetupCall;
		GetWorldTimerManager().SetTimer(THandle_PostMinigameSetupCall, this, &ASBoardGS::PostMinigameSetup, 2.0f, false);
		return;
	}

	//remove all players' ability to interact with the game
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		//have all server/clients possess the turn order pawn and allow for selection turn order cards
		ThisPC->SwitchInteractability(false);
		ThisPC->SetAbleToBePaused(false);
	}

	//name each player state to use in setting turn order
	if (GetWorld()->IsServer())
	{
		
		for (int i = 0; i < PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

			if (i == 0)
			{
				ThisPS->SetPlayerName("P1");
				ThisPS->SetIsMyTurn(true);
			}
			else if (i == 1)
			{
				ThisPS->SetPlayerName("P2");
				ThisPS->SetIsMyTurn(false);
			}
			else if (i == 2)
			{
				ThisPS->SetPlayerName("P3");
				ThisPS->SetIsMyTurn(false);
			}
			else if (i == 3)
			{
				ThisPS->SetPlayerName("P4");
				ThisPS->SetIsMyTurn(false);
			}
		}
	}

	//set a default turn order and set the first player's turn
	TurnOrderArray[0] = 1;
	TurnOrderArray[1] = 2;
	TurnOrderArray[2] = 3;
	TurnOrderArray[3] = 4;

	CurrentPlayerTurn = TurnOrderArray[0];

	InitialSetup();
}

//Perform all the following actions upon loading into the level at the start of a game
void ASBoardGS::InitialSetup()
{
	//find and destroy all the turn order cards (if TO was not skipped)
	TArray<AActor*> FoundCards;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTOCards::StaticClass(), FoundCards);
	for (int i = 0; i < FoundCards.Num(); ++i)
	{ FoundCards[i]->Destroy(); }

	//spawn the player ships and store their references in the array
	if (GetWorld()->IsServer())
	{
		FVector Play1Location;
		Play1Location.Set(-3500.0f, 0.0f, 0.0f);
		FVector Play2Location;
		Play2Location.Set(0.0f, 3500.0f, 0.0f);
		FVector Play3Location;
		Play3Location.Set(3500.0f, 0.0f, 0.0f);
		FVector Play4Location;
		Play4Location.Set(0.0f, -3500.0f, 0.0f);

		FRotator SetAtZeroRotation;
		SetAtZeroRotation.ZeroRotator;

		//spawn the player ships and store references to their pawns
		PlayerPawns[0] = GetWorld()->SpawnActor<APawn>(PlayerShipClass, Play1Location, SetAtZeroRotation);
		PlayerPawns[1] = GetWorld()->SpawnActor<APawn>(PlayerShipClass, Play2Location, SetAtZeroRotation);
		PlayerPawns[2] = GetWorld()->SpawnActor<APawn>(PlayerShipClass, Play3Location, SetAtZeroRotation);
		PlayerPawns[3] = GetWorld()->SpawnActor<APawn>(PlayerShipClass, Play4Location, SetAtZeroRotation);
	
		//assign the initial states of the player state identifier variables
		InitialPlayerStates();
	}

	//store the player colors in array for later access
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

		if (ThisPS->GetPlayerName() == "P1")
		{ ColorArray[0] = ThisPS->ReturnPlayerColor(); }
		else if (ThisPS->GetPlayerName() == "P2")
		{ ColorArray[1] = ThisPS->ReturnPlayerColor(); }
		else if (ThisPS->GetPlayerName() == "P3")
		{ ColorArray[2] = ThisPS->ReturnPlayerColor(); }
		else if (ThisPS->GetPlayerName() == "P4")
		{ ColorArray[3] = ThisPS->ReturnPlayerColor(); }
	}

	FTimerHandle THandle_InitialSetupPossesion;
	FTimerHandle THandle_InitialSetupSetView;

	//give time for spawning and replication before having players possess their ships and set the camera view
	GetWorldTimerManager().SetTimer(THandle_InitialSetupPossesion, this, &ASBoardGS::PossessPlayerPawns, 0.2f, false);
	GetWorldTimerManager().SetTimer(THandle_InitialSetupSetView, this, &ASBoardGS::SetViewToCurrentSpace, 1.0f, false);

	//iterate through the PCs and have them move the camera and make their widget managers (currently only widget on the manager is the InGame widget for turn order and current player turn)
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		if (CurrentPlayerTurn == 1 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P1" ||
			CurrentPlayerTurn == 2 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P2" ||
			CurrentPlayerTurn == 3 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P3" ||
			CurrentPlayerTurn == 4 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P4")
		{
			ThisPC->SwitchInteractability(true);
			ThisPC->SetAbleToBePaused(false);
		}

		ThisPC->CreateManagerWidget(ColorArray);
	}
}

//setup the board the way it was before the level transition to the minigame
void ASBoardGS::PostMinigameSetup()
{
	//recall the turn order and the player who's turn it was
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

		if(ThisPS->GetPlayerName() == "P1")
		{ TurnOrderArray[ThisPS->ReturnPlayerTurnNum()] = 1; }
		else if(ThisPS->GetPlayerName() == "P2")
		{ TurnOrderArray[ThisPS->ReturnPlayerTurnNum()] = 2; }
		else if(ThisPS->GetPlayerName() == "P3")
		{ TurnOrderArray[ThisPS->ReturnPlayerTurnNum()] = 3; }
		else if(ThisPS->GetPlayerName() == "P4")
		{ TurnOrderArray[ThisPS->ReturnPlayerTurnNum()] = 4; }

		if (ThisPS->ReturnIsMyTurn())
		{
			if(ThisPS->GetPlayerName() == "P1")
			{ CurrentPlayerTurn = TurnOrderArray[ThisPS->ReturnPlayerTurnNum()]; }
			else if(ThisPS->GetPlayerName() == "P2")
			{ CurrentPlayerTurn = TurnOrderArray[ThisPS->ReturnPlayerTurnNum()]; }
			else if(ThisPS->GetPlayerName() == "P3")
			{ CurrentPlayerTurn = TurnOrderArray[ThisPS->ReturnPlayerTurnNum()]; }
			else if(ThisPS->GetPlayerName() == "P4")
			{ CurrentPlayerTurn = TurnOrderArray[ThisPS->ReturnPlayerTurnNum()]; }

			TurnCount = ThisPS->ReturnNumOfTurns();
		}
	}

	//respawn the player ships and re-assign them to each player
	if (GetWorld()->IsServer())
	{
		FVector Play1Location;
		Play1Location.Set(-3500.0f, 0.0f, 0.0f);
		FVector Play2Location;
		Play2Location.Set(0.0f, 3500.0f, 0.0f);
		FVector Play3Location;
		Play3Location.Set(3500.0f, 0.0f, 0.0f);
		FVector Play4Location;
		Play4Location.Set(0.0f, -3500.0f, 0.0f);

		FRotator SetAtZeroRotation;
		SetAtZeroRotation.ZeroRotator;

		//spawn the player ships and store references to their pawns
		PlayerPawns[0] = GetWorld()->SpawnActor<APawn>(PlayerShipClass, Play1Location, SetAtZeroRotation);
		PlayerPawns[1] = GetWorld()->SpawnActor<APawn>(PlayerShipClass, Play2Location, SetAtZeroRotation);
		PlayerPawns[2] = GetWorld()->SpawnActor<APawn>(PlayerShipClass, Play3Location, SetAtZeroRotation);
		PlayerPawns[3] = GetWorld()->SpawnActor<APawn>(PlayerShipClass, Play4Location, SetAtZeroRotation);

		for (int i = 0; i < PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

			if (i == 0)
			{ ThisPS->SetPlayerPawn(PlayerPawns[0]); }
			else if (i == 1)
			{ ThisPS->SetPlayerPawn(PlayerPawns[1]); }
			else if (i == 2)
			{ ThisPS->SetPlayerPawn(PlayerPawns[2]); }
			else if (i == 3)
			{ ThisPS->SetPlayerPawn(PlayerPawns[3]); }
		}
	}

	//allow time for spawning to go into affect before having players possess them
	FTimerHandle THandle_PostMinigamePossession;
	GetWorldTimerManager().SetTimer(THandle_PostMinigamePossession, this, &ASBoardGS::PossessPlayerPawns, 2.0f, false);

	FTimerHandle THandle_AssignPlayerSpace_1;
	FTimerHandle THandle_AssignPlayerSpace_2;
	FTimerHandle THandle_AssignPlayerSpace_3;
	FTimerHandle THandle_AssignPlayerSpace_4;

	//store an array to match each player's color to their name and then assign them to their previous space
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

		if (ThisPS->GetPlayerName() == "P1")
		{
			ColorArray[0] = ThisPS->ReturnPlayerColor();
			GetWorldTimerManager().SetTimer(THandle_AssignPlayerSpace_1, ThisPS, &ASPlayerState::AssignPlayerSpace, 5.0f, false);
		}
		else if (ThisPS->GetPlayerName() == "P2")
		{
			ColorArray[1] = ThisPS->ReturnPlayerColor();
			GetWorldTimerManager().SetTimer(THandle_AssignPlayerSpace_2, ThisPS, &ASPlayerState::AssignPlayerSpace, 5.0f, false);
		}
		else if (ThisPS->GetPlayerName() == "P3")
		{
			ColorArray[2] = ThisPS->ReturnPlayerColor();
			GetWorldTimerManager().SetTimer(THandle_AssignPlayerSpace_3, ThisPS, &ASPlayerState::AssignPlayerSpace, 5.0f, false);
		}
		else if (ThisPS->GetPlayerName() == "P4")
		{
			ColorArray[3] = ThisPS->ReturnPlayerColor();
			GetWorldTimerManager().SetTimer(THandle_AssignPlayerSpace_4, ThisPS, &ASPlayerState::AssignPlayerSpace, 5.0f, false);
		}
	}

	//allow the other methods to complete before adjusting the camera to the space of the player who's turn it is
	FTimerHandle THandle_PostMinigameSetView;
	GetWorldTimerManager().SetTimer(THandle_PostMinigameSetView, this, &ASBoardGS::SetViewToCurrentSpace, 6.0f, false);

	//allow interactability for the player who's turn it currently is and then create the ManagerWidget
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		if (CurrentPlayerTurn == 1 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P1" ||
			CurrentPlayerTurn == 2 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P2" ||
			CurrentPlayerTurn == 3 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P3" ||
			CurrentPlayerTurn == 4 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P4")
		{
			ThisPC->SwitchInteractability(true);
			ThisPC->SetAbleToBePaused(false);
		}

		ThisPC->CreateManagerWidget(ColorArray);
	}

	//allow a delay for the board to reset and replicate before moving the next player's turn
	FTimerHandle THandle_PostMinigameNextTurn;
	GetWorldTimerManager().SetTimer(THandle_PostMinigameNextTurn, this, &ASBoardGS::NextTurnInOrder, 8.0f, false);
}

//assign the pawns, colors, and turn order information on the player state
void ASBoardGS::InitialPlayerStates()
{
	//store the players' colors, ship pawns, cards, and set who's turn it is
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

		if (i == 0)
		{
			ThisPS->SetPlayerPawn(PlayerPawns[0]);
			ThisPS->SetPlayerColor(FLinearColor::Red);
		}
		else if (i == 1)
		{
			ThisPS->SetPlayerPawn(PlayerPawns[1]);
			ThisPS->SetPlayerColor(FLinearColor::Green);
		}
		else if (i == 2)
		{
			ThisPS->SetPlayerPawn(PlayerPawns[2]);
			ThisPS->SetPlayerColor(FLinearColor::Blue);
		}
		else if (i == 3)
		{
			ThisPS->SetPlayerPawn(PlayerPawns[3]);
			ThisPS->SetPlayerColor(FLinearColor::Yellow);
		}

		for (int k = 0; k < 3; ++k)
		{
			ThisPS->SetPlayerCards(k, CardRandomizer());
			ThisPS->SetPlayerPaints(k, "Default");
		}

		for (int j = 0; j < TurnOrderArray.Num(); ++j)
		{
			if (TurnOrderArray[j] == 1 && ThisPS->GetPlayerName() == "P1" ||
				TurnOrderArray[j] == 2 && ThisPS->GetPlayerName() == "P2" ||
				TurnOrderArray[j] == 3 && ThisPS->GetPlayerName() == "P3" ||
				TurnOrderArray[j] == 4 && ThisPS->GetPlayerName() == "P4")
			{ ThisPS->SetPlayerTurnNum(j); }
		}

		if (TurnOrderArray[0] == i)
		{ ThisPS->SetIsMyTurn(true); }
		else
		{ ThisPS->SetIsMyTurn(false); }
	}
}

//pick and return a random card
FString ASBoardGS::CardRandomizer()
{
	int CardForRandomizer = FMath::RandRange(1, 5);
	if (CardForRandomizer == 1)
	{ return "1T3"; }
	else if (CardForRandomizer == 2)
	{ return "2T4"; }
	else if (CardForRandomizer == 3)
	{ return "3T5"; }
	else if (CardForRandomizer == 4)
	{ return "4T6"; }
	else if (CardForRandomizer == 5)
	{ return "1T6"; }
	return "What??";
}

//find the cards for the current player and spawn them in
void ASBoardGS::CurrentCardSetup()
{
	if (GetWorld()->IsServer())
	{
		//find the player name that matches the current player turn and store a reference to their cards
		for (int i = 0; i < PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

			if (CurrentPlayerTurn == 1 && ThisPS->GetPlayerName() == "P1" ||
				CurrentPlayerTurn == 2 && ThisPS->GetPlayerName() == "P2" ||
				CurrentPlayerTurn == 3 && ThisPS->GetPlayerName() == "P3" ||
				CurrentPlayerTurn == 4 && ThisPS->GetPlayerName() == "P4")
			{
				CurrentCards = ThisPS->ReturnPlayerCards();
				CurrentPaints = ThisPS->ReturnPlayerPaints();
			}
		}

		//spawn in the cards and netmulticast it on each client
		CardSpawning(CurrentCards[0], CurrentCards[1], CurrentCards[2], CurrentPaints[0], CurrentPaints[1], CurrentPaints[2]);
	}
}

//spawn in the cards based on the player who's turn it currently is, then assign all their variables
void ASBoardGS::CardSpawning_Implementation(const FString &C1, const FString &C2, const FString &C3, const FString &P1, const FString &P2, const FString &P3)
{
	//ensure that the bool to check if a player is using a wormhole space is reset
	if(GetWorld()->IsServer())
	{ IsOnWormhole = false; }

	FVector ViewLocation;
	FRotator ViewRotation;
	APlayerCameraManager* CamManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	CamManager->GetCameraViewPoint(ViewLocation, ViewRotation);

	//CamXAxis multiplier determines how far the cards spawn from the camera (FRONT/BACK)
	//CamYAxis multiplier determines distance the cards are from each other (LEFT/RIGHT)
	//CamZAxis multiplier determines how high the cards are in frame (UP/DOWN)
	FVector CamXAxis = CamManager->GetActorForwardVector();
	FVector CamYAxis = CamManager->GetActorRightVector();
	FVector CamZAxis = CamManager->GetActorUpVector();

	CardLocation_1 = ViewLocation + (CamXAxis * 30.0f + CamYAxis * -10.0f + CamZAxis * -15.0f);
	CardLocation_2 = ViewLocation + (CamXAxis * 30.0f + CamYAxis * 0.0f + CamZAxis * -15.0f);
	CardLocation_3 = ViewLocation + (CamXAxis * 30.0f + CamYAxis * 10.0f + CamZAxis * -15.0f);

	//FRotator has its vector with Pitch (Y), Yaw (Z), and Roll (X)
	FRotator CamBasedRotation = FRotator(0.0f, ViewRotation.Yaw - 90.0f, ViewRotation.Roll + 45.0f);

	Card_1 = Cast<ASCardSystem>(GetWorld()->SpawnActor<AActor>(CardSystemClass, CardLocation_1, CamBasedRotation));
	Card_2 = Cast<ASCardSystem>(GetWorld()->SpawnActor<AActor>(CardSystemClass, CardLocation_2, CamBasedRotation));
	Card_3 = Cast<ASCardSystem>(GetWorld()->SpawnActor<AActor>(CardSystemClass, CardLocation_3, CamBasedRotation));

	//set the different movement locations for the cards when interacted it the mouse
	Card_1->SetCenterScreen(ViewLocation + (CamXAxis * 20.0f));
	Card_2->SetCenterScreen(ViewLocation + (CamXAxis * 20.0f));
	Card_3->SetCenterScreen(ViewLocation + (CamXAxis * 20.0f));

	Card_1->SetHoverLocation(ViewLocation + (CamXAxis * 20.0f + CamYAxis * -7.5f + CamZAxis * -7.5f));
	Card_2->SetHoverLocation(ViewLocation + (CamXAxis * 20.0f + CamYAxis * 0.0f + CamZAxis * -7.5f));
	Card_3->SetHoverLocation(ViewLocation + (CamXAxis * 20.0f + CamYAxis * 7.5f + CamZAxis * -7.5f));

	//set the current cards on the screen based on the current player's turn
	Card_1->SetCardString(C1);
	Card_2->SetCardString(C2);
	Card_3->SetCardString(C3);

	//set the card's index based on its position on screen
	Card_1->SetCardIndex(1);
	Card_2->SetCardIndex(2);
	Card_3->SetCardIndex(3);

	//set the card's paint value
	Card_1->SetCardPaint(P1);
	Card_2->SetCardPaint(P2);
	Card_3->SetCardPaint(P3);

	//update the mesh of the cards based on their value
	Card_1->SetCardMesh();
	Card_2->SetCardMesh();
	Card_3->SetCardMesh();

	//the cards are too close to the camera and need to be scaled down
	Card_1->SetActorRelativeScale3D(Card_1->GetActorRelativeScale3D() * 0.0125f);
	Card_2->SetActorRelativeScale3D(Card_2->GetActorRelativeScale3D() * 0.0125f);
	Card_3->SetActorRelativeScale3D(Card_3->GetActorRelativeScale3D() * 0.0125f);

	//set mouse interactability and pause ability only for the current player
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		if (CurrentPlayerTurn == 1 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P1" ||
			CurrentPlayerTurn == 2 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P2" ||
			CurrentPlayerTurn == 3 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P3" ||
			CurrentPlayerTurn == 4 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P4")
		{
			ThisPC->SwitchInteractability(true);
			ThisPC->SetAbleToBePaused(true);
		}
		else
		{
			ThisPC->SwitchInteractability(false);
			ThisPC->SetAbleToBePaused(false);
		}
	}
}

//take the information from the selected/chosen card and replicate it to the same card on each client/server
void ASBoardGS::ReplicateBoolToCards_Implementation(int CIndex, const FString& BoolToUpdate, bool SetStatus)
{
	//access all the cards on screen
	TArray<AActor*> ArrayOfCards;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASCardSystem::StaticClass(), ArrayOfCards);

	//sort through the array of all the cards in the scene
	for (int i = 0; i < ArrayOfCards.Num(); ++i)
	{
		ASCardSystem* ThisCard = Cast<ASCardSystem>(ArrayOfCards[i]);

		//once the correct card is found, pass the bool updates to all cards at that index on all computers
		if (ThisCard->ReturnCardIndex() == CIndex)
		{
			if (BoolToUpdate == "HoverOff")
			{ ThisCard->SetHoverOff(SetStatus); }
			else if (BoolToUpdate == "HoverOn")
			{ ThisCard->SetHoverOn(SetStatus); }
			else if (BoolToUpdate == "AbleToHover")
			{ ThisCard->SetAbleToHover(SetStatus); }
			else if (BoolToUpdate == "AdjustSpinSpeed")
			{
				//if this bool is updated, then a card has been selected, sort through and destroy every card except the one chosen
				ThisCard->SetAdjustSpinSpeed(SetStatus);
				if (GetWorld()->IsServer())
				{
					for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
					{
						APlayerController* PC = It->Get();
						ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);
						
						if (CurrentPlayerTurn == 1 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P1" ||
							CurrentPlayerTurn == 2 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P2" ||
							CurrentPlayerTurn == 3 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P3" ||
							CurrentPlayerTurn == 4 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P4")
						{
							ThisPC->GetPlayerState<ASPlayerState>()->SetPlayerCards(ThisCard->ReturnCardIndex() - 1, CardRandomizer());
						}

						ThisPC->SwitchInteractability(false);
						ThisPC->SetAbleToBePaused(false);
					}
				}
				
				//destroy the cards not chosen
				for (int j = 0; j < ArrayOfCards.Num(); ++j)
				{
					if (j != i)
					{ ArrayOfCards[j]->Destroy(); }
				}
				break;
			}
		}
	}
}

//UNREAL RECOMMENDS POSSESSION ONLY ON SERVER, POSSIBLY COME BACK AND MOVE THIS TO GAMEMODE LATER
void ASBoardGS::PossessPlayerPawns()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		ThisPC->Possess(ThisPC->GetPlayerState<ASPlayerState>()->ReturnPlayerPawn());
	}
}

//set all players' cameras to the space that the current player is on
void ASBoardGS::SetViewToCurrentSpace()
{
	if (GetWorld()->IsServer())
	{
		APawn* CurrentPlayerPawn = nullptr;

		//access the pawn of the current player
		for (int i = 0; i < PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

			if (CurrentPlayerTurn == 1 && ThisPS->GetPlayerName() == "P1" ||
				CurrentPlayerTurn == 2 && ThisPS->GetPlayerName() == "P2" ||
				CurrentPlayerTurn == 3 && ThisPS->GetPlayerName() == "P3" ||
				CurrentPlayerTurn == 4 && ThisPS->GetPlayerName() == "P4")
			{
				CurrentPlayerPawn = ThisPS->ReturnPlayerPawn();
			}
		}

		//set the camera to the spaced stored on the current player's pawn
		if (CurrentPlayerPawn != nullptr)
		{
			FocusCameraOnActor(Cast<ASPlayerPawn>(CurrentPlayerPawn)->ReturnCurrentSpace(), 0.5f);
		}
	}
}

//move the pawn of the current player to their next space
void ASBoardGS::PawnMovement(int MovementNumber)
{
	if (GetWorld()->IsServer())
	{
		APawn* CurrentPlayerPawn = nullptr;

		//match the player name to the current player turn and move that ship based on the card chosen
		for (int i = 0; i < PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

			if (CurrentPlayerTurn == 1 && ThisPS->GetPlayerName() == "P1" ||
				CurrentPlayerTurn == 2 && ThisPS->GetPlayerName() == "P2" ||
				CurrentPlayerTurn == 3 && ThisPS->GetPlayerName() == "P3" ||
				CurrentPlayerTurn == 4 && ThisPS->GetPlayerName() == "P4")
			{
				CurrentPlayerPawn = Cast<ASPlayerPawn>(ThisPS->ReturnPlayerPawn());

				for (int j = 0; j < abs(MovementNumber); ++j)
				{
					if(MovementNumber > 0)
					{ Cast<ASPlayerPawn>(ThisPS->ReturnPlayerPawn())->BeginMoveForward(); }
					else if(MovementNumber < 0)
					{ Cast<ASPlayerPawn>(ThisPS->ReturnPlayerPawn())->BeginMoveBackward(); }
				}
				break;
			}
		}

		//relocate the camera focus to the moving ship
		FocusCameraOnActor(CurrentPlayerPawn, 0.5f);
	}
}

//once a player finishes moving to their new spot, move to the next player in the turn order
void ASBoardGS::NextTurnInOrder()
{
	//if not resetting turn order, move to the next player in the order and update the card widget	
	if (CurrentPlayerTurn == TurnOrderArray[0])
	{ CurrentPlayerTurn = TurnOrderArray[1]; }
	else if (CurrentPlayerTurn == TurnOrderArray[1])
	{ CurrentPlayerTurn = TurnOrderArray[2]; }
	else if (CurrentPlayerTurn == TurnOrderArray[2])
	{ CurrentPlayerTurn = TurnOrderArray[3]; }
	else if (CurrentPlayerTurn == TurnOrderArray[3])
	{ CurrentPlayerTurn = TurnOrderArray[0];

		//move to the next turn
		--TurnCount;
	}

	//update widget with the turn count and the current player's turn
	UpdateTurnWidgetOnPC();

	//set the current player's turn and update the turncounter for everyone
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

		if ((CurrentPlayerTurn == 1 && ThisPS->GetPlayerName() == "P1") ||
			(CurrentPlayerTurn == 2 && ThisPS->GetPlayerName() == "P2") ||
			(CurrentPlayerTurn == 3 && ThisPS->GetPlayerName() == "P3") ||
			(CurrentPlayerTurn == 4 && ThisPS->GetPlayerName() == "P4"))
		{
			ThisPS->SetIsMyTurn(true);
		}
		else
		{
			ThisPS->SetIsMyTurn(false);
		}

		ThisPS->UpdateNumOfTurns(TurnCount);
	}

	//reset the red card if it was used
	SetRedCardActive(false);

	//assign the camera to the space of the next player
	SetViewToCurrentSpace();
}

//update the references to each player's current score
void ASBoardGS::UpdatePlayerScoreArray(FString PlayerToUpdate, float ScoreToUpdate)
{
	if(PlayerToUpdate == "P1")
	{ PlayerScoreArray[0] = ScoreToUpdate; }
	else if(PlayerToUpdate == "P2")
	{ PlayerScoreArray[1] = ScoreToUpdate; }
	else if(PlayerToUpdate == "P3")
	{ PlayerScoreArray[2] = ScoreToUpdate; }
	else if(PlayerToUpdate == "P4")
	{ PlayerScoreArray[3] = ScoreToUpdate; }
}

//expand the sun to the next level
void ASBoardGS::UpdateNovaSun_Implementation()
{
	TArray<AActor*> NovaSun;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSun::StaticClass(), NovaSun);
	Cast<ASSun>(NovaSun[0])->UpdateSunWithGS();
}

//pause/unpause the game
void ASBoardGS::PMenuSwitch(bool IsGamePaused)
{
	//if the game is currently paused, unpause it
	if (IsGamePaused)
	{
		//ensure that all players are in possession of their ships
		PossessPlayerPawns();
		
		//return the camera to the current player's current space
		SetViewToCurrentSpace();

		//COME BACK AFTER REWORKING WIDGET COMPLETED METHOD TO AVOID HAVING TO USE THIS ITERATOR
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

			ThisPC->SetbIsPaused(false);
			ThisPC->SetShowPlayerLocations(false);
			ThisPC->ChangeWidgets(1);
		}

		//bring back the cards of the current player's turn (wait until camera is setup for cards to be placed correctly)
		FTimerHandle THandle_UnPauseCardSetup;
		GetWorldTimerManager().SetTimer(THandle_UnPauseCardSetup, this, &ASBoardGS::CurrentCardSetup, 2.0f, false);
		
		//destroy the camera created for the pause menu (wait to ensure that camera is no longer in use)
		FTimerHandle THandle_UnPauseDestroyCam;
		GetWorldTimerManager().SetTimer(THandle_UnPauseDestroyCam, this, &ASBoardGS::DestroyFreeCam, 4.0f, false);
	}
	//if the game is currently unpaused, pause it
	else
	{
		//make sure the scores for each player are updated for the score widget
		if (GetWorld()->IsServer())
		{
			for (int i = 0; i < PlayerArray.Num(); ++i)
			{
				ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

				UpdatePlayerScoreArray(ThisPS->GetPlayerName(), ThisPS->ReturnPlayerScore());
			}
		}

		//destroy any cards onscreen
		if (Card_1)
		{ Card_1->Destroy(); }
		if (Card_2)
		{ Card_2->Destroy(); }
		if (Card_3)
		{ Card_3->Destroy(); }

		//if a pause screen camera doesn't exist, spawn one
		if (!FreeCamPawn)
		{
			//access the current camera's current view
			APlayerCameraManager* CamManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
			FVector CamLocation = CamManager->GetCameraLocation();
			FVector ForwardFacingVector = CamLocation + CamManager->GetActorForwardVector();

			//spawn the pause screen camera
			FreeCamPawn = GetWorld()->SpawnActor<APawn>(FreeCameraClass, FVector(ForwardFacingVector.X, ForwardFacingVector.Y, ForwardFacingVector.Z + 9000.0f), FRotator(270.0f, 0.0f, 0.0f));

			//give time for camera to spawn before switching viewpoint and possesing it
			FTimerHandle THandle_PauseCamSpawn;
			GetWorldTimerManager().SetTimer(THandle_PauseCamSpawn, this, &ASBoardGS::DelayedCameraSpawn, 2.0f, false);
		}
	}
}

//what until camera actors are spawned before switching to the pause menu
void ASBoardGS::DelayedCameraSpawn()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		//set the bool to represent that the game is currently paused
		ThisPC->SetbIsPaused(true);

		//switch to the ScoreWidget
		ThisPC->ChangeWidgets(0);

		//change the camera view and establish the inidicators for showing offscreen players
		ThisPC->SetViewTargetWithBlend(FreeCamPawn, 2.0f);
		ThisPC->SetShowPlayerLocations(true);

		//have the current player be able to control the camera and unpause
		if (CurrentPlayerTurn == 1 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P1" ||
			CurrentPlayerTurn == 2 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P2" ||
			CurrentPlayerTurn == 3 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P3" ||
			CurrentPlayerTurn == 4 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P4")
		{
			ThisPC->Possess(FreeCamPawn);
			ThisPC->SetAbleToBePaused(true);
		}
	}
}

//destroy the pause screen camera and return the player's ability to pause the game
void ASBoardGS::DestroyFreeCam()
{
	//disconnect the camera from the reference
	FreeCamPawn = NULL;

	//find the camera and destroy it
	TArray<AActor*> FoundCams;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASFreeCamera::StaticClass(), FoundCams);

	if (FoundCams.Num() > 0)
	{
		for (int i = 0; i < FoundCams.Num(); ++i)
		{ FoundCams[i]->Destroy(); }
	}

	//allow the current player to be able to pause the game
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		if (CurrentPlayerTurn == 1 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P1" ||
			CurrentPlayerTurn == 2 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P2" ||
			CurrentPlayerTurn == 3 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P3" ||
			CurrentPlayerTurn == 4 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P4")
		{
			ThisPC->SetAbleToBePaused(true);
		}
	}
}

//iterate through the PCs and have each computer update their widgets with turn information
void ASBoardGS::UpdateTurnWidgetOnPC()
{
	if (GetWorld()->IsServer())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

			if (CurrentPlayerTurn == 1)
			{ ThisPC->UpdateTurnWidget(TurnCount, "P1", 1); }
			else if (CurrentPlayerTurn == 2)
			{ ThisPC->UpdateTurnWidget(TurnCount, "P2", 2); }
			else if (CurrentPlayerTurn == 3)
			{ ThisPC->UpdateTurnWidget(TurnCount, "P3", 3); }
			else if (CurrentPlayerTurn == 4)
			{ ThisPC->UpdateTurnWidget(TurnCount, "P4", 4); }
		}
	}

	//bring up the cards of the player who's turn it currently is
	FTimerHandle THandle_CardSetup;
	GetWorldTimerManager().SetTimer(THandle_CardSetup, this, &ASBoardGS::CurrentCardSetup, 2.0f, false);
}

//take action based on if a player has made their lane choice or is landing at their lane choice
void ASBoardGS::LaneChoice_Implementation(bool LeavingOrLanding)
{
	//update the widget based on if the player is leaving the sun space or landing at the lane gate
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		LeavingOrLanding ? ThisPC->ChangeWidgets(1) : ThisPC->ChangeWidgets(3);
	}

	//if a player is leaving the sun space, focus the camera on their pawn
	if (LeavingOrLanding)
	{
		APawn* ThisPawn = nullptr;

		for (int i = 0; i < PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

			if (CurrentPlayerTurn == 1 && ThisPS->GetPlayerName() == "P1" ||
				CurrentPlayerTurn == 2 && ThisPS->GetPlayerName() == "P2" ||
				CurrentPlayerTurn == 3 && ThisPS->GetPlayerName() == "P3" ||
				CurrentPlayerTurn == 4 && ThisPS->GetPlayerName() == "P4")
			{
				ThisPawn = ThisPS->ReturnPlayerPawn();
			}
		}

		FocusCameraOnActor(ThisPawn, 0.5f);
	}
	//if the player is landing on their lane gate choice, move to the next player
	else
	{
		NextOnSunWidget();
		SetViewToCurrentSpace();
	}
}

//at the start of the game, move through the players after each one makes their lane choice
void ASBoardGS::NextOnSunWidget()
{
	FTimerHandle THandle_WidgetResetNextTurn;

	//if the players are picking their lanes at the start of the game, each player needs to make a choice
	if (StartingTurn)
	{
		if (CurrentPlayerTurn == TurnOrderArray[0])
		{ CurrentPlayerTurn = TurnOrderArray[1]; }
		else if (CurrentPlayerTurn == TurnOrderArray[1])
		{ CurrentPlayerTurn = TurnOrderArray[2]; }
		else if (CurrentPlayerTurn == TurnOrderArray[2])
		{ CurrentPlayerTurn = TurnOrderArray[3]; }
		else if (CurrentPlayerTurn == TurnOrderArray[3])
		{ 
			ReplicateWidgetCompleted();
			
			GetWorldTimerManager().SetTimer(THandle_WidgetResetNextTurn, this, &ASBoardGS::NextTurnInOrder, 1.0f, false);
			StartingTurn = false;
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			//grab the controller and cast it to the custom PC
			APlayerController* PC = It->Get();
			ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

			if (CurrentPlayerTurn == 1 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P1" ||
				CurrentPlayerTurn == 2 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P2" ||
				CurrentPlayerTurn == 3 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P3" ||
				CurrentPlayerTurn == 4 && ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName() == "P4")
			{ ThisPC->SwitchInteractability(true); }
			else
			{ ThisPC->SwitchInteractability(false); }
		}
	}
	//if this is not the starting turn, then only the current player needs to make their lane choice
	else
	{
		ReplicateWidgetCompleted();
		GetWorldTimerManager().SetTimer(THandle_WidgetResetNextTurn, this, &ASBoardGS::NextTurnInOrder, 1.0f, false);
	}
	
}

//return to the InGameWidget and move to the next turn in order
void ASBoardGS::ReplicateWidgetCompleted_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		ThisPC->ChangeWidgets(1);
	}
}

//when a player wins the game, find the winning player and update the WinGameWidget
void ASBoardGS::InitiateWinScreen()
{
	if (GetWorld()->IsServer())
	{
		float WinScore = 0;
		FString WinName;
		FLinearColor WinColor;

		//find which player has the highest number of points and set them as the winner
		for (int i = 0; i < PlayerArray.Num(); ++i)
		{
			ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

			if (ThisPS->ReturnPlayerScore() > WinScore)
			{
				WinName = ThisPS->GetPlayerName();
				WinColor = ThisPS->ReturnPlayerColor();
			}
		}

		//disable interactability and change to the WinGameWidget
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			//grab the controller and cast it to the custom PC
			APlayerController* PC = It->Get();
			ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

			ThisPC->SwitchInteractability(false);
			ThisPC->SetAbleToBePaused(false);

			ThisPC->ChangeWidgets(4);
			ThisPC->UpdateWinningWidget(WinName, WinColor);
		}
	}
}

//access each of the planet's values and store them in an array
TArray<int> ASBoardGS::ReturnPlanetScores()
{
	TArray<AActor*> ArrayOfPlanets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASPlanet::StaticClass(), ArrayOfPlanets);

	if (ArrayOfPlanets.Num() > 0)
	{
		for (int i = 0; i < ArrayOfPlanets.Num(); ++i)
		{
			ASPlanet* ThisPlanet = Cast<ASPlanet>(ArrayOfPlanets[i]);
			if (ThisPlanet->ReturnLaneNum() == 1)
			{ PlanetCounts[0] = ThisPlanet->GetPodBank(); }
			else if (ThisPlanet->ReturnLaneNum() == 2)
			{ PlanetCounts[1] = ThisPlanet->GetPodBank(); }
			else if (ThisPlanet->ReturnLaneNum() == 3)
			{ PlanetCounts[2] = ThisPlanet->GetPodBank(); }
			else if (ThisPlanet->ReturnLaneNum() == 4)
			{ PlanetCounts[3] = ThisPlanet->GetPodBank(); }
			else if (ThisPlanet->ReturnLaneNum() == 5)
			{ PlanetCounts[4] = ThisPlanet->GetPodBank(); }
		}
	}

	return PlanetCounts;
}

//set the camera for all players on the passed in actor
void ASBoardGS::FocusCameraOnActor(AActor* ActorToFocusOn, float TimeToBlend)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		ThisPC->SetViewTargetWithBlend(ActorToFocusOn, TimeToBlend);
	}
}

//Move the player to the wormhole space in the lane they chose
void ASBoardGS::WormHoleMovement(APawn* PawnToMove, int LaneToMoveTo, int CurrentLane)
{
	int NewSpaceNum = 0;

	ASPlayerPawn* ThisPawn = Cast<ASPlayerPawn>(PawnToMove);

	TArray<AActor*> ArrayOfWormholes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASWormhole::StaticClass(), ArrayOfWormholes);

	//increment the number of players at the new space and set the variables for it
	if (ArrayOfWormholes.Num() > 0)
	{
		for (int i = 0; i < ArrayOfWormholes.Num(); ++i)
		{
			ASWormhole* ThisWormhole = Cast<ASWormhole>(ArrayOfWormholes[i]);

			if (ThisWormhole->ReturnLaneNum() == LaneToMoveTo)
			{
				if (ThisWormhole->ReturnPawnCount() == 0)
				{
					PawnToMove->SetActorLocation(ThisWormhole->GetTransitionPointA());
					ThisWormhole->SetPawnCount(1);
				}
				else if (ThisWormhole->ReturnPawnCount() == 1)
				{
					PawnToMove->SetActorLocation(ThisWormhole->GetTransitionPointB());
					ThisWormhole->SetPawnCount(2);
				}
				else if (ThisWormhole->ReturnPawnCount() == 2)
				{
					PawnToMove->SetActorLocation(ThisWormhole->GetTransitionPointC());
					ThisWormhole->SetPawnCount(3);
				}
				else if (ThisWormhole->ReturnPawnCount() == 3)
				{
					PawnToMove->SetActorLocation(ThisWormhole->GetTransitionPointD());
					ThisWormhole->SetPawnCount(4);
				}
				
				ThisPawn->SetCurrentSpace(ThisWormhole);
				ThisWormhole->SetOrbitingPlayer(ThisPawn);
				
				NewSpaceNum = ThisWormhole->ReturnSpaceNum();
			}
			else if (ThisWormhole->ReturnLaneNum() == CurrentLane)
			{
				ThisWormhole->SetOrbitingPlayer(nullptr);
			}
		}
	}

	//update the ID variables of the space/lane the player is currently on
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ASPlayerState* ThisPS = Cast<ASPlayerState>(PlayerArray[i]);

		if (CurrentPlayerTurn == 1 && ThisPS->GetPlayerName() == "P1" ||
			CurrentPlayerTurn == 2 && ThisPS->GetPlayerName() == "P2" ||
			CurrentPlayerTurn == 3 && ThisPS->GetPlayerName() == "P3" ||
			CurrentPlayerTurn == 4 && ThisPS->GetPlayerName() == "P4")
		{
			ThisPS->SetCurrentSpaceNum(NewSpaceNum);
			ThisPS->SetPlayerLane(LaneToMoveTo);
		}
	}
}