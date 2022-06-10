// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SSpace.h"
#include "SPlanet.generated.h"

UCLASS()
class COLLABTUT_API ASPlanet : public ASSpace
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPlanet();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//setters and getters for the PodBank
	UFUNCTION(Server, Reliable)
	void SetPodBank(int PodUpdate);
	int GetPodBank() { return PodBank; };

private:

	//value to attribute to player score when they land on this planet
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Space\|CurrentValue")
	int PodBank = 10;

	//update score of the player and reset the PodBank
	UFUNCTION(Server, Reliable)
	void DepositPodBank();

	//reference to the sun space to reset the player's position on the board
	UPROPERTY(EditInstanceOnly, Category = "Space\|PlanetSpace")
	class ASSunSpace* SunSpace;
};
