// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API USScoreWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//update the text to represent the score of each player
	void UpdatePlayersScores(TArray<float> PScores);

	//update the colors of each text to represent which player it belongs to
	void UpdatePlayerColors(TArray<FLinearColor> WColors);

	//update the icons that represent the locations of offscreen players
	void UpdateLocationIndicators(FLinearColor ColorToUpdate, FVector2D LocationToUpdate, bool OnScreen);

private:

	//text block for the score of each player
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Score_P1;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Score_P2;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Score_P3;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Score_P4;

	//icon for the location of each player
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_B;
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_G;
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_R;
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Y;
};
