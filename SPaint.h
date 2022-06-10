// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SPaint.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASPaint : public ASSpace
{
	GENERATED_BODY()

public:

	ASPaint();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	//assign a random color/effect to a random card of the player that lands on this space
	void PaintCard();

};
