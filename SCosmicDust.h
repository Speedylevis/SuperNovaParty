// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SCosmicDust.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASCosmicDust : public ASSpace
{
	GENERATED_BODY()
	
public:

	ASCosmicDust();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	//planet to affect the PodBank of when landing on this space
	UPROPERTY(EditInstanceOnly, Category = "Space\|CosmicDustSpace")
	class ASPlanet* ConnectedPlanet;

	//increase the PodBank of the connected planet
	void IncrementPodCount();
};
