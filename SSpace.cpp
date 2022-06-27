// Fill out your copyright notice in the Description page of Project Settings.

#include "SSpace.h"

//ENGINE HEADERS
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SPlayerPawn.h"
#include "SPlayerState.h"

//COMPONENT HEADERS
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASSpace::ASSpace()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsActive = false;

	bDumpSpaceSet = false;
	bStartSpaceSet = false;

	bReplicates = true;
	bAlwaysRelevant = true;

	//Create scene components and hierarchy
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	StaticMeshComp->SetupAttachment(SceneComp);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(StaticMeshComp);
	
	SpringArm->SetRelativeRotation(FRotator(330.0f, 0.0f, 0.0f));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->TargetArmLength = SpringArmLength;

	TestCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TestCameraComp"));
	TestCameraComp->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);
}

void ASSpace::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASSpace, bIsActive);
	DOREPLIFETIME(ASSpace, OrbitingPlayer);
}

// Called when the game starts or when spawned
void ASSpace::BeginPlay()
{
	Super::BeginPlay();

	//create transition points based on actor's location
	TransitionPointA = FVector(0 - 24 * 97.916931f, 0, 0);
	TransitionPointB = FVector(0 + 24* 97.916931f, 0, 0);

	TransitionPointC = FVector(0, 0 - 24 * 97.916931f, 0);
	TransitionPointD = FVector(0, 0 + 24* 97.916931f, 0);
}

// Called every frame
void ASSpace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Makes sure that Pawns don't spawn on top of each other
	if (!bStartSpaceSet)
	{
		if (bIsStartingSpace)
		{
			if (!GetWorld()->GetGameState<ASBoardGS>()->ReturnPostMinigame())
			{
				//set all player pawns to the starting space
				for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
				{
					APawn* Pawn = It->Get();

					ASPlayerPawn* PlayerPawn = Cast<ASPlayerPawn>(Pawn);

					if (PlayerPawn && PlayerPawn->ReturnCurrentSpace() == nullptr)
					{
						PlayerPawn->SetCurrentSpace(this);

						if (PawnCount == 0)
						{
							PlayerPawn->SetActorLocation(GetTransitionPointA());
							PawnCount++;
							break;
						}
						if (PawnCount == 1)
						{
							PlayerPawn->SetActorLocation(GetTransitionPointB());
							PawnCount++;
							break;
						}
						if (PawnCount == 2)
						{
							PlayerPawn->SetActorLocation(GetTransitionPointC());
							PawnCount++;
							break;
						}
						if (PawnCount == 3)
						{
							PlayerPawn->SetActorLocation(GetTransitionPointD());
							PawnCount++;

							bStartSpaceSet = true;
							break;
						}
					}
				}
			}
			else
			{
				bStartSpaceSet = true;
			}
		}
	}
}

//assign this space to a player and increase the pawn count
void ASSpace::AssignPlayerToSpace(APawn* PawnToSet)
{
	ASPlayerPawn* ThisPawn = Cast<ASPlayerPawn>(PawnToSet);

	ThisPawn->SetCurrentSpace(this);
	if (PawnCount == 0)
	{
		OrbitingPlayer = ThisPawn;
		ThisPawn->SetActorLocation(GetTransitionPointA());
		PawnCount++;
	}
	else if (PawnCount == 1)
	{
		OrbitingPlayer = ThisPawn;
		ThisPawn->SetActorLocation(GetTransitionPointB());
		PawnCount++;
	}
	else if (PawnCount == 2)
	{
		OrbitingPlayer = ThisPawn;
		ThisPawn->SetActorLocation(GetTransitionPointC());
		PawnCount++;
	}
	else if (PawnCount == 3)
	{
		OrbitingPlayer = ThisPawn;
		ThisPawn->SetActorLocation(GetTransitionPointD());
		PawnCount++;
	}
}

//set the orbiting player and send them to the dump if there is already a player here
void ASSpace::SetOrbitingPlayer(APawn* ThisOrbitingPlayer)
{
	if (OrbitingPlayer == nullptr)
	{
		OrbitingPlayer = ThisOrbitingPlayer;
		ThisOrbitingPlayer->SetActorLocation(GetTransitionPointA());
		PawnCount++;
	}
	else
	{
		// If Player 2 lands on same space, send Player 1 to Dump (unless under special circumstances: shortcut, wormhole, etc.)
		if (!bIsStartingSpace)
		{
			if (bNoDumpingAllowed)
			{
				if (PawnCount == 1)
				{
					ThisOrbitingPlayer->SetActorLocation(GetTransitionPointB());
					PawnCount++;
				}
				else if (PawnCount == 2)
				{
					ThisOrbitingPlayer->SetActorLocation(GetTransitionPointC());
					PawnCount++;
				}
				else if (PawnCount == 3)
				{
					ThisOrbitingPlayer->SetActorLocation(GetTransitionPointD());
					PawnCount++;
				}
			}
			else
			{
				DumpPlayer();
				ThisOrbitingPlayer->SetActorLocation(GetTransitionPointA());
			}
		}

		OrbitingPlayer = ThisOrbitingPlayer;
		
	}

	bIsActive = true;

	//if this space is a lane gate, call the method to reset on the GS
	if (bIsLaneGate)
	{
		GetWorld()->GetGameState<ASBoardGS>()->LaneChoice(false);
	}
}

//send a player to the dump space
void ASSpace::DumpPlayer()
{
	ASPlayerPawn* DumpedPlayer = Cast<ASPlayerPawn>(OrbitingPlayer);
	ASSpace* ThisDumpSpace = Cast<ASSpace>(DumpingSpace);

	if (DumpedPlayer && ThisDumpSpace)
	{
		DumpedPlayer->SetCurrentSpace(DumpingSpace);
		DumpedPlayer->SetActorLocation(ThisDumpSpace->GetTransitionPointA());
		ThisDumpSpace->SetOrbitingPlayer(DumpedPlayer);

		GetWorld()->GetFirstPlayerController<ASPlayerController>()->SetPlayerToDumpSpace();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(4812, 20.0f, FColor::Cyan, "Dump Space broken");
	}
}

void ASSpace::NextTurn()
{
	//when player lands, move to next in turn order
	ASBoardGS* ThisGS = GetWorld()->GetGameState<ASBoardGS>();
	FTimerHandle THandle_SpaceNextTurnCall;

	GetWorldTimerManager().SetTimer(THandle_SpaceNextTurnCall, ThisGS, &ASBoardGS::NextTurnInOrder, 1.0f, false);
}

