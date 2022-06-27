// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SDuelChoice.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API USDuelChoice : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetDuelOptions(FString B1, FLinearColor C1, FString B2, FLinearColor C2, FString B3, FLinearColor C3);

protected:

	virtual bool Initialize();

private:

	//buttons and text on widgets to bind to
	UPROPERTY(meta = (BindWidget))
	class UButton* DCButton_1;
	UPROPERTY(meta = (BindWidget))
	class UButton* DCButton_2;
	UPROPERTY(meta = (BindWidget))
	class UButton* DCButton_3;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DCText_1;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DCText_2;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DCText_3;

	//functions tied to button presses
	UFUNCTION()
	void Button_1_Chosen();
	UFUNCTION()
	void Button_2_Chosen();
	UFUNCTION()
	void Button_3_Chosen();

	
};
