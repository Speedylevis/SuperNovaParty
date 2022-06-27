// Fill out your copyright notice in the Description page of Project Settings.


#include "SDuelSpace.h"

//ENGINE HEADERS
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"

//CLASS HEADERS
#include "BoardGM.h"
#include "SPlayerController.h"

ASDuelSpace::ASDuelSpace()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create the scene components for the actor
	SecondSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SecondSceneComp->SetupAttachment(RootComponent);

	ShipMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMeshComp->SetupAttachment(SecondSceneComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh> DuelShipMesh(TEXT("/Game/Meshes/Spaces/DuelSpace"));
	ShipMesh = DuelShipMesh.Object;

	//access and assign the materials from their location in the folder
	ConstructorHelpers::FObjectFinder<UMaterialInstance> Neon_B_Class(TEXT("/Game/Materials/Neon/Neon_Player_Ship_Blue"));
	Neon_B = Neon_B_Class.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInstance> Neon_G_Class(TEXT("/Game/Materials/Neon/Neon_Player_Ship_Green"));
	Neon_G = Neon_G_Class.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInstance> Neon_R_Class(TEXT("/Game/Materials/Neon/Neon_Player_Ship_Red"));
	Neon_R = Neon_R_Class.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInstance> Neon_Y_Class(TEXT("/Game/Materials/Neon/Neon_Player_Ship_Yellow"));
	Neon_Y = Neon_Y_Class.Object;

	ConstructorHelpers::FObjectFinder<UMaterialInstance> Aluminum_B_Class(TEXT("/Game/Materials/Aluminium/M_Aluminium_Blue"));
	Aluminum_B = Aluminum_B_Class.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInstance> Aluminum_G_Class(TEXT("/Game/Materials/Aluminium/M_Aluminium_Green"));
	Aluminum_G = Aluminum_G_Class.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInstance> Aluminum_R_Class(TEXT("/Game/Materials/Aluminium/M_Aluminium_Red"));
	Aluminum_R = Aluminum_R_Class.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInstance> Aluminum_Y_Class(TEXT("/Game/Materials/Aluminium/M_Aluminium_Yellow"));
	Aluminum_Y = Aluminum_Y_Class.Object;

	//assign the size of the arrays
	NeonArray.SetNum(4);
	AluminumArray.SetNum(4);
}

void ASDuelSpace::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDuelSpace, ColorToSet_1);
	DOREPLIFETIME(ASDuelSpace, ColorToSet_2);
}

void ASDuelSpace::BeginPlay()
{
	Super::BeginPlay();

	ShipMeshComp->SetStaticMesh(ShipMesh);

	NeonArray[0] = Neon_B;
	NeonArray[1] = Neon_G;
	NeonArray[2] = Neon_R;
	NeonArray[3] = Neon_Y;

	AluminumArray[0] = Aluminum_B;
	AluminumArray[1] = Aluminum_G;
	AluminumArray[2] = Aluminum_R;
	AluminumArray[3] = Aluminum_Y;

	AssignRandomColorNumber();
}

void ASDuelSpace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		Duel();
	}
}

//TBD: have current player pick an opponent and move them to a 2player minigame
void ASDuelSpace::Duel_Implementation()
{
	bIsActive = false;
	
	GetWorld()->GetFirstPlayerController<ASPlayerController>()->CallDuelUpdate();
}

//randomize the two integers used for changing the colors of the ships of this space
void ASDuelSpace::AssignRandomColorNumber()
{
	if (GetWorld()->IsServer())
	{
		int FirstRandomNumber = FMath::RandRange(1, 3);
		int SecondRandomNumber = FMath::RandRange(1, 3);

		//if color picked is the same as the current color, increment it
		if (FirstRandomNumber >= ColorToSet_1)
		{ ++FirstRandomNumber; }

		if (SecondRandomNumber >= ColorToSet_2)
		{ ++SecondRandomNumber; }

		//if colors end up being the same, run the method again until they are different
		if (FirstRandomNumber == SecondRandomNumber)
		{ AssignRandomColorNumber(); }
		//once color parameters are met (different from their current and different from each other), initiate the color switch
		else
		{
			ColorToSet_1 = FirstRandomNumber;
			ColorToSet_2 = SecondRandomNumber;

			FTimerHandle THandle_SetColor;
			GetWorldTimerManager().SetTimer(THandle_SetColor, this, &ASDuelSpace::SwitchColor, 1.0f, false);
		}
	}
}

//set the materials based on the new color integers
void ASDuelSpace::SwitchColor_Implementation()
{
	for (int i = 1; i < 5; ++i)
	{
		if (ColorToSet_1 == i)
		{
			ShipMesh->SetMaterial(2, NeonArray[i - 1]);
			ShipMesh->SetMaterial(3, AluminumArray[i - 1]);
		}

		if (ColorToSet_2 == i)
		{
			ShipMesh->SetMaterial(4, NeonArray[i - 1]);
			ShipMesh->SetMaterial(5, AluminumArray[i - 1]);
		}
	}

	ShipMeshComp->SetStaticMesh(ShipMesh);

	//after a preset time, change the colors again
	FTimerHandle THandle_AssignColorNum;
	GetWorldTimerManager().SetTimer(THandle_AssignColorNum, this, &ASDuelSpace::AssignRandomColorNumber, 1.0f, false);
}