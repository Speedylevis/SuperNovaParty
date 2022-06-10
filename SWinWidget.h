// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWinWidget.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API USWinWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//update the text to represent the winning player
	void UpdateWinningText(FString WinnerName, FLinearColor WinnerColor);

private:

	//binding of the text block to be edited in C++
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WinningPlayer;

};
