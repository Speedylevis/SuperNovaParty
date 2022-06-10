// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWormholeWidget.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API USWormholeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//update the widget to show where each player is and the score of each lane
	void SetWormholeText();

	//deactivate the button the current player is on
	void DeactivateButton(int ButtonToDeactivate);

protected:

	virtual bool Initialize();

private:

	//array of the value of each planet
	TArray<int> PlanetBanks;

	//bindings of buttons to tie them to functions
	UPROPERTY(meta = (BindWidget))
	class UButton* LaneChoice_1;
	UPROPERTY(meta = (BindWidget))
	class UButton* LaneChoice_2;
	UPROPERTY(meta = (BindWidget))
	class UButton* LaneChoice_3;
	UPROPERTY(meta = (BindWidget))
	class UButton* LaneChoice_4;
	UPROPERTY(meta = (BindWidget))
	class UButton* LaneChoice_5;

	//bindings of the text to represent the players in each lane
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNames_L1;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNames_L2;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNames_L3;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNames_L4;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNames_L5;

	//bindings of the text to represent the value of each planet
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlanetScore_L1;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlanetScore_L2;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlanetScore_L3;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlanetScore_L4;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlanetScore_L5;

	//functions tied to the press of each button
	UFUNCTION()
	void Wormhole_L1();
	UFUNCTION()
	void Wormhole_L2();
	UFUNCTION()
	void Wormhole_L3();
	UFUNCTION()
	void Wormhole_L4();
	UFUNCTION()
	void Wormhole_L5();

	//calls movement of player on the server based on their lane choice
	void WormholeChosen(int WormholeChoice);
};
