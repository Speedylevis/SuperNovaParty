// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SWormhole.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASWormhole : public ASSpace
{
	GENERATED_BODY()

public:
	ASWormhole();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	//bring up the wormhole widget to allow player to choose what lane to move to
	void SelectRealm();

};
