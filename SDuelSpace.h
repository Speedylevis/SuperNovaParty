// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SDuelSpace.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASDuelSpace : public ASSpace
{
	GENERATED_BODY()
	
public:
	ASDuelSpace();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//TBD: have current player pick an opponent and move them to a 2player minigame
	void Duel();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Root Component for other components to attach to for this space
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* RootComp;

	//reference variable for the static mesh of the duel space
	UPROPERTY(BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ShipMesh;

	//randomize the integers that represent to colors to set the ships to
	void AssignRandomColorNumber();
	//set the colors of the ships based on the randomized integers
	void SwitchColor();

	//integers to represent what colors to set the ships of this space
	int ColorToSet_1;
	int ColorToSet_2;

	//references to the neon materials of the ships
	UMaterialInstance* Neon_B;
	UMaterialInstance* Neon_G;
	UMaterialInstance* Neon_R;
	UMaterialInstance* Neon_Y;

	//references the aluminum materials of the ships
	UMaterialInstance* Aluminum_B;
	UMaterialInstance* Aluminum_G;
	UMaterialInstance* Aluminum_R;
	UMaterialInstance* Aluminum_Y;

	//arrays to condense and work through the sets of materials
	TArray<UMaterialInstance*> NeonArray;
	TArray<UMaterialInstance*> AluminumArray;

	//time between switching colors of the space
	UPROPERTY(EditAnywhere, Category = "Color Switching")
	float ColorSwitchSpeed = 1.0f;
};
