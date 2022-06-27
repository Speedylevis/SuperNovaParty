// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SGambleWidget.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API USGambleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	//initialize method for button implementation
	virtual bool Initialize();

private:
	//number of pods that player bets
	int8 Bet;

	void Gamble(int8 ThisBet);

	//bind the buttons of the widget for pod amount selection
	UPROPERTY(meta = (BindWidget))
	class UButton* OnePod_Button;
	UPROPERTY(meta = (BindWidget))
	class UButton* TwoPod_Button;
	UPROPERTY(meta = (BindWidget))
	class UButton* ThreePod_Button;

	//bind the text for the buttons
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OnePod_Text;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TwoPod_Text;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ThreePod_Text;

	//functions bound to buttons
	UFUNCTION()
	void BetOnePod();
	UFUNCTION()
	void BetTwoPod();
	UFUNCTION()
	void BetThreePod();
};
