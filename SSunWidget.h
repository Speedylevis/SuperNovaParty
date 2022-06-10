// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SSunWidget.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API USSunWidget : public UUserWidget
{
	GENERATED_BODY()

protected: 
	//intialize method for button implementation
	virtual bool Initialize();

private:

	bool bSunSpaceSet;

	//lane that player selects
	int8 Lane;

	void MoveToLane(int8 Lane);

	void MovePlayer(AActor* ThisMovingPlayer, int8 LaneNumber);

	TArray<int> PlanetBanks;

	UFUNCTION(BlueprintCallable)
	void SetPlayersText();

	UFUNCTION(BlueprintCallable)
	void SetPlanetsText();

	UFUNCTION()
	void SelectLaneOne();

	UFUNCTION()
	void SelectLaneTwo();

	UFUNCTION()
	void SelectLaneThree();

	UFUNCTION()
	void SelectLaneFour();

	UFUNCTION()
	void SelectLaneFive();

	//bind the buttons of the widget
	UPROPERTY(meta = (BindWidget))
	class UButton* LaneOne_Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* LaneTwo_Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* LaneThree_Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* LaneFour_Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* LaneFive_Button;

	//bind the text for the buttons
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneOne_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneTwo_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneThree_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneFour_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneFive_Text;

	//bind the text for lane's player info

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneOne_Pod_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneTwo_Pod_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneThree_Pod_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneFour_Pod_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneFive_Pod_Text;

	//bind the text for the planet pod count
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneOne_Players_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneTwo_Players_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneThree_Players_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneFour_Players_Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LaneFive_Players_Text;
};
