// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SAsteroid.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASAsteroid : public ASSpace
{
	GENERATED_BODY()
	
public:

	ASAsteroid();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	//planet to affect the PodBank of when landing on this space
	UPROPERTY(EditInstanceOnly, Category = "Space\|AsteroidSpace")
	class ASPlanet* ConnectedPlanet;

	//lower the PodBank of the connected planet
	void DecrementPodCount();
};
