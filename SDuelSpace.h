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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Root Component for other components to attach to for this space
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* SecondSceneComp;

	//reference variable for the static mesh of the duel space
	UPROPERTY(BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ShipMeshComp;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	UStaticMesh* ShipMesh;

	//randomize the integers that represent to colors to set the ships to
	void AssignRandomColorNumber();
	//set the colors of the ships based on the randomized integers
	UFUNCTION(NetMultiCast, Reliable)
	void SwitchColor();

	//integers to represent what colors to set the ships of this space
	UPROPERTY(Replicated)
	int ColorToSet_1;
	UPROPERTY(Replicated)
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

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	//have current player pick an opponent and move them to a 2player minigame
	UFUNCTION(Server, Reliable)
	void Duel();
};
