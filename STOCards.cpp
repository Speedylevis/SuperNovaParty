// Fill out your copyright notice in the Description page of Project Settings.


#include "STOCards.h"

//CLASS HEADERS
#include "SPlayerController.h"

// Sets default values
ASTOCards::ASTOCards()
{
	//create the scene component for this objects static mesh
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComp;

	//grab the different meshes that this card could be set as
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshClass1(TEXT("/Game/Meshes/Cards/TOCards/TOCard_1"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshClass2(TEXT("/Game/Meshes/Cards/TOCards/TOCard_2"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshClass3(TEXT("/Game/Meshes/Cards/TOCards/TOCard_3"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshClass4(TEXT("/Game/Meshes/Cards/TOCards/TOCard_4"));

	CardMesh_1 = StaticMeshClass1.Object;
	CardMesh_2 = StaticMeshClass2.Object;
	CardMesh_3 = StaticMeshClass3.Object;
	CardMesh_4 = StaticMeshClass4.Object;
}

// Called when the game starts or when spawned
void ASTOCards::BeginPlay()
{
	Super::BeginPlay();

	//connect the mouse interaction to the assigned function
	OnClicked.AddDynamic(this, &ASTOCards::ClickMethod);
}

//set the scale of this card and the mesh it uses based on its value
void ASTOCards::SetupCard()
{
	SetActorScale3D(FVector(0.01f, 0.01f, 0.01f));

	if (CardValue == "1")
	{ StaticMeshComp->SetStaticMesh(CardMesh_1); }
	else if (CardValue == "2")
	{ StaticMeshComp->SetStaticMesh(CardMesh_2); }
	else if (CardValue == "3")
	{ StaticMeshComp->SetStaticMesh(CardMesh_3); }
	else if (CardValue == "4")
	{ StaticMeshComp->SetStaticMesh(CardMesh_4); }
}

void ASTOCards::ClickMethod(AActor* TouchedActor, FKey ButtonPressed)
{
	//if this card has not already been chosen
	if (AbleToBeClicked)
	{
		//when clicked, pass this value upwards and deactivate it
		GetWorld()->GetFirstPlayerController<ASPlayerController>()->TOCardChosen(CardValue);
		//Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->TOCardChosen(CardValue);
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;
		GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = false;
	}
}

