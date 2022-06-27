// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetManager.h"

//change the widget based on the passed in value
void UWidgetManager::SwitchWidgets(int WidgetToSwitchTo)
{
	/* WIDGET DIRECTORY
	*	0 = ScoreWidget
	*	1 = InGameWidget
	*	2 = GambleWidget
	*	3 = SunWidget
	*	4 = WinScreenWidget
	*	5 = WormholeWidget
	*	6 = DuelWidget
	*/

	WidgetSwitch->SetActiveWidgetIndex(WidgetToSwitchTo);
}