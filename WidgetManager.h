// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "WidgetManager.generated.h"

/**
 * 
 */
UCLASS()
class COLLABTUT_API UWidgetManager : public UUserWidget
{
	GENERATED_BODY()
	
public:

	//return the index of the widget currently active on the widget switcher
	int32 CurrentIndexReturn() { return WidgetSwitch->GetActiveWidgetIndex(); };
	//return the widget that is currently active on the widget switcher
	UWidget* SwitchReturn() { return WidgetSwitch->GetActiveWidget(); };

	//change the widget based on the passed in value
	void SwitchWidgets(int WidgetToSwitchTo);

private:

	//binding for the widget switcher to alternate widgets
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitch;
};
