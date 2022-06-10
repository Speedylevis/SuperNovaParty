// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSpace.h"
#include "SDump.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API ASDump : public ASSpace
{
	GENERATED_BODY()
	
public:

	ASDump();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
