// Fill out your copyright notice in the Description page of Project Settings.


#include "SCardSystem.h"

//ENGINE HEADERS
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//PROJECT HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"

// Sets default values
ASCardSystem::ASCardSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create scene comps and store references
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComp;

	RotMoveComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
	RotMoveComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);

	//find the files for the meshes in the Content folder and pass their reference to be stored in a variable
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_1(TEXT("/Game/Meshes/Cards/Precision_Cards/Card_1"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_2(TEXT("/Game/Meshes/Cards/Precision_Cards/Card_2"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_3(TEXT("/Game/Meshes/Cards/Precision_Cards/Card_3"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_4(TEXT("/Game/Meshes/Cards/Precision_Cards/Card_4"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_5(TEXT("/Game/Meshes/Cards/Precision_Cards/Card_5"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_6(TEXT("/Game/Meshes/Cards/Precision_Cards/Card_6"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_1T3(TEXT("/Game/Meshes/Cards/Ranged_Cards/Card_1T3"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_1T6(TEXT("/Game/Meshes/Cards/Ranged_Cards/Card_1T6"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_2T4(TEXT("/Game/Meshes/Cards/Ranged_Cards/Card_2T4"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_3T5(TEXT("/Game/Meshes/Cards/Ranged_Cards/Card_3T5"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh_4T6(TEXT("/Game/Meshes/Cards/Ranged_Cards/Card_4T6"));

	CardMesh_1 = Mesh_1.Object;
	CardMesh_2 = Mesh_2.Object;
	CardMesh_3 = Mesh_3.Object;
	CardMesh_4 = Mesh_4.Object;
	CardMesh_5 = Mesh_5.Object;
	CardMesh_6 = Mesh_6.Object;

	CardMesh_1T3 = Mesh_1T3.Object;
	CardMesh_1T6 = Mesh_1T6.Object;
	CardMesh_2T4 = Mesh_2T4.Object;
	CardMesh_3T5 = Mesh_3T5.Object;
	CardMesh_4T6 = Mesh_4T6.Object;
}

// Called when the game starts or when spawned
void ASCardSystem::BeginPlay()
{
	Super::BeginPlay();

	//orient the card to face the camera, disable rotation until selected
	RotMoveComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	//SetActorRotation(FRotator(0.0f, 270.0f, 90.0f));

	//bindings for methods related to mouse interaction with cards
	OnClicked.AddDynamic(this, &ASCardSystem::ClickMethod);
	OnBeginCursorOver.AddDynamic(this, &ASCardSystem::HoverMethod);
	OnEndCursorOver.AddDynamic(this, &ASCardSystem::HoverMethod_End);

	//store the starting location of this card to reset it
	OriginalLocation = GetActorLocation();
}

// Called every frame
void ASCardSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//when a card has been selected
	if (AdjustSpinSpeed)
	{
		//move it to the center of the screen
		if ((CenterScreen - GetActorLocation()).Size() >= 0.0f)
		{
			FVector NewVect = FMath::VInterpConstantTo(GetActorLocation(), CenterScreen, DeltaTime, CardMoveSpeed);
			SetActorLocation(NewVect);
		}

		//gradually increase rotation speed
		CardRotSpeed += IncrementSpeed * DeltaTime;
		RotMoveComp->RotationRate = FRotator(0.0f, CardRotSpeed, 0.0f);

		//once card reaches max rotation speed, have it randomly select from the range of this card's value
		if (CardRotSpeed >= MaxRotSpeed)
		{
			if (CardString == "1T3")
			{
				MovementNumber = FMath::RandRange(1, 3);
				if (CardPaint == "Yellow")
				{ MovementNumber += FMath::RandRange(1, 3); }
				else if(CardPaint == "Blue")
				{ MovementNumber = -MovementNumber; }
			}
			else if (CardString == "1T6")
			{
				MovementNumber = FMath::RandRange(1, 6);
				if (CardPaint == "Yellow")
				{ MovementNumber += FMath::RandRange(1, 6); }
				else if(CardPaint == "Blue")
				{ MovementNumber = -MovementNumber; }
			}
			else if (CardString == "2T4")
			{
				MovementNumber = FMath::RandRange(2, 4);
				if (CardPaint == "Yellow")
				{ MovementNumber += FMath::RandRange(2, 4); }
				else if(CardPaint == "Blue")
				{ MovementNumber = -MovementNumber; }
			}
			else if (CardString == "3T5")
			{
				MovementNumber = FMath::RandRange(3, 5);
				if (CardPaint == "Yellow")
				{ MovementNumber += FMath::RandRange(3, 5); }
				else if(CardPaint == "Blue")
				{ MovementNumber = -MovementNumber; }
			}
			else if (CardString == "4T6")
			{
				MovementNumber = FMath::RandRange(4, 6);
				if (CardPaint == "Yellow")
				{ MovementNumber += FMath::RandRange(4, 6); }
				else if(CardPaint == "Blue")
				{ MovementNumber = -MovementNumber; }
			}

			GEngine->AddOnScreenDebugMessage(9123, 5.0f, FColor::Emerald, "Move Number : " + FString::FromInt(MovementNumber));

			//CHANGE THE CARD TO REFLECT THE FINAL MOVEMENT DECISION
			/*
			SetCardString(FString::FromInt(MovementNumber));

			//update the card to be the randomly selected value
			SetCardMesh();
			*/

			//dynMaterial_1 = UMaterialInstanceDynamic::Create(StaticMeshComp->GetMaterial(0), this);
			//dynMaterial_2 = UMaterialInstanceDynamic::Create(StaticMeshComp->GetMaterial(1), this);
			//dynMaterial_3 = UMaterialInstanceDynamic::Create(StaticMeshComp->GetMaterial(2), this);


			//disable rotation and face the new card at the camera
			RotMoveComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
			SetActorRelativeRotation(FRotator(0.0f, 270.0F, 90.0f));

			AdjustSpinSpeed = false;
			DecrementSpinSpeed = true;
		}
	}
	if (DecrementSpinSpeed)
	{
		//spawn the explosion particle emmmiter, pass the value into the gamestate to move the pawn, and destroy the card
		if (SpawnOnce)
		{
			SpawnOnce = false;
			const FTransform& ThisTransform = GetActorTransform();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CardEmitter, ThisTransform);

			//If player has a red  card, set the bool active on the GameState (will be checked when landing on a space where this card can activate)
			if(CardPaint == "Red")
			{ GetWorld()->GetGameState<ASBoardGS>()->SetRedCardActive(true); }
			GetWorld()->GetGameState<ASBoardGS>()->PawnMovement(MovementNumber);

			//destroy will be replaced with the commented code below to have a fade out effect
			Destroy();
		}

		//FADE OUT CARD ONCE PARTICLE EFFECT GOES OFF
		/*
		TempMaterialOpacity -= CardDisappearSpeed * DeltaTime;

		dynMaterial_1->SetScalarParameterValue(FName(TEXT("Opacity")), TempMaterialOpacity);
		dynMaterial_2->SetScalarParameterValue(FName(TEXT("Opacity")), TempMaterialOpacity);
		dynMaterial_3->SetScalarParameterValue(FName(TEXT("Opacity")), TempMaterialOpacity);

		StaticMeshComp->SetMaterial(0, dynMaterial_1);
		StaticMeshComp->SetMaterial(1, dynMaterial_2);
		StaticMeshComp->SetMaterial(2, dynMaterial_3);

		if (TempMaterialOpacity <= 0)
		{
			ActivateCard();

			DecrementSpinSpeed = false;
			Destroy();
		}
		*/
	}

	//when the mouse moves over the card, move the card to the hover location
	if (HoverOn)
	{
		if ((HoverLocation - GetActorLocation()).Size() >= 0.0f)
		{
			FVector NewVect = FMath::VInterpConstantTo(GetActorLocation(), HoverLocation, DeltaTime, CardMoveSpeed);
			SetActorLocation(NewVect);
		}
	}
	//when the mouse is not hovering over this card, move it back to its original location
	else if (HoverOff)
	{
		if ((OriginalLocation - GetActorLocation()).Size() >= 0.0f)
		{
			FVector NewVect = FMath::VInterpConstantTo(GetActorLocation(), OriginalLocation, DeltaTime, CardMoveSpeed);
			SetActorLocation(NewVect);
		}
	}
}

//set the mesh of the card based on its value
void ASCardSystem::SetCardMesh()
{
	if (CardString == "1")
	{ StaticMeshComp->SetStaticMesh(CardMesh_1); }
	else if (CardString == "2")
	{ StaticMeshComp->SetStaticMesh(CardMesh_2); }
	else if (CardString == "3")
	{ StaticMeshComp->SetStaticMesh(CardMesh_3); }
	else if (CardString == "4")
	{ StaticMeshComp->SetStaticMesh(CardMesh_4); }
	else if (CardString == "5")
	{ StaticMeshComp->SetStaticMesh(CardMesh_5); }
	else if (CardString == "6")
	{ StaticMeshComp->SetStaticMesh(CardMesh_6); }
	else if (CardString == "1T3")
	{ StaticMeshComp->SetStaticMesh(CardMesh_1T3); }
	else if (CardString == "1T6")
	{ StaticMeshComp->SetStaticMesh(CardMesh_1T6); }
	else if (CardString == "2T4")
	{ StaticMeshComp->SetStaticMesh(CardMesh_2T4); }
	else if (CardString == "3T5")
	{ StaticMeshComp->SetStaticMesh(CardMesh_3T5); }
	else if (CardString == "4T6")
	{ StaticMeshComp->SetStaticMesh(CardMesh_4T6); }

	// Update the card color with the correct paint/ability
	PaintDynamicMaterialInst = UMaterialInstanceDynamic::Create(StaticMeshComp->GetMaterial(0), StaticMeshComp);

	if (CardPaint == "Default")
	{ 

	}
	else if (CardPaint == "Red")
	{ 
		PaintDynamicMaterialInst->SetVectorParameterValue(FName(TEXT("Color")), FColor::Red);
		StaticMeshComp->SetMaterial(0, PaintDynamicMaterialInst);
	}
	else if (CardPaint == "Blue")
	{ 
		PaintDynamicMaterialInst->SetVectorParameterValue(FName(TEXT("Color")), FColor::Blue);
		StaticMeshComp->SetMaterial(0, PaintDynamicMaterialInst);
	}
	else if (CardPaint == "Yellow")
	{ 
		PaintDynamicMaterialInst->SetVectorParameterValue(FName(TEXT("Color")), FColor::Yellow);
		StaticMeshComp->SetMaterial(0, PaintDynamicMaterialInst);
	}
}

//methods for mouse interactability

void ASCardSystem::ClickMethod(AActor* TouchedActor, FKey ButtonPressed)
{
	Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->PassMouseInputToGS(CardIndex, "AbleToHover", false);
	Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->PassMouseInputToGS(CardIndex, "HoverOff", false);
	Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->PassMouseInputToGS(CardIndex, "HoverOn", false);
	Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->PassMouseInputToGS(CardIndex, "AdjustSpinSpeed", true);
}

void ASCardSystem::HoverMethod(AActor* HoveredActor)
{
	if (AbleToHover)
	{
		Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->PassMouseInputToGS(CardIndex, "HoverOff", false);
		Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->PassMouseInputToGS(CardIndex, "HoverOn", true);
	}
}

void ASCardSystem::HoverMethod_End(AActor* HoveredActor)
{
	if (AbleToHover)
	{
		Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->PassMouseInputToGS(CardIndex, "HoverOn", false);
		Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->PassMouseInputToGS(CardIndex, "HoverOff", true);
	}
}