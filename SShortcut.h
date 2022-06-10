// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SShortcut.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASShortcut : public ASSpace
{
	GENERATED_BODY()

public:
	ASShortcut();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	//set the player's location to the shortcut destination
	void Shortcut();

	//the space this shortcut sends the player to when they land here
	UPROPERTY(EditInstanceOnly, Category = "Space\|Shortcut")
	AActor* ShorcutDestination;
};
