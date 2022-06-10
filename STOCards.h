// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STOCards.generated.h"

UCLASS()
class COLLABTUT_API ASTOCards : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASTOCards();
	
	//setter and getter for the string value that defines which turn order card this is
	void SetCardValue(FString CardToSet) { CardValue = CardToSet; };
	FString ReturnCardValue() { return CardValue; };

	//once chosen this card is unable to be interacted with by the playercontroller
	void DisableAbilityToClick() { AbleToBeClicked = false; }

	//set the mesh and scale of the spawned card
	void SetupCard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	//scene components for the static mesh of this card
	UStaticMeshComponent* StaticMeshComp;
	UStaticMesh* CardMesh_1;
	UStaticMesh* CardMesh_2;
	UStaticMesh* CardMesh_3;
	UStaticMesh* CardMesh_4;

	//string value that determines which turn order card this is
	FString CardValue;

private:

	//method called when clicking on this card
	UFUNCTION()
	void ClickMethod(AActor* TouchedActor, FKey ButtonPressed);

	//bool to prevent interaction with this card after it has been chosen
	bool AbleToBeClicked = true;
};
