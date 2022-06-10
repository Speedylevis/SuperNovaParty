// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API UInGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//update the widget to show who's turn it is and what turn it is
	void SetPlayerTurn(FString PTurn);
	void SetTurnNumber(FString NTurn);

private:

	//bind the text for the representation of the current player and the current turn
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerTurn;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TurnNumber;
};
