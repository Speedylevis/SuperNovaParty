// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SGamble.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASGamble : public ASSpace
{
	GENERATED_BODY()

public:

	ASGamble();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	//activate the gamble widget
	void GambleWidget();

};
