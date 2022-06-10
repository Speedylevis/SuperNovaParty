// Fill out your copyright notice in the Description page of Project Settings.


#include "SWormhole.h"

//ENGINE HEADERS
#include "Blueprint/WidgetBlueprintLibrary.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SWormholeWidget.h"
#include "WidgetManager.h"


ASWormhole::ASWormhole()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASWormhole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		SelectRealm();
		bIsActive = false;		
	}
}

//switch the widget to the wormhole widget and deactivate the wormhole the player is currently at
void ASWormhole::SelectRealm()
{
	//look for all widgets on screen, by using a manager with a widget switcher we will only have one widget on screen and so we are able to use this function
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	if (FoundWidgets.Num() > 0)
	{
		//prevent code from reactivating when teleporting to another wormhole
		if (!GetWorld()->GetGameState<ASBoardGS>()->ReturnIsOnWormhole())
		{
			GetWorld()->GetGameState<ASBoardGS>()->SetIsOnWormhole(true);

			//switch the widget to the wormhole widget
			UWidgetManager* TheManager = Cast<UWidgetManager>(FoundWidgets[0]);

			TheManager->SwitchWidgets(5);

			Cast<USWormholeWidget>(TheManager->SwitchReturn())->SetWormholeText();

			//deactivate the current wormhole to force player to move to another lane
			int WormholeLane = 0;

			if (ReturnLaneNum() == 1)
			{ WormholeLane = 1; }
			else if (ReturnLaneNum() == 2)
			{ WormholeLane = 2; }
			else if (ReturnLaneNum() == 3)
			{ WormholeLane = 3; }
			else if (ReturnLaneNum() == 4)
			{ WormholeLane = 4; }
			else if (ReturnLaneNum() == 5)
			{ WormholeLane = 5; }

			Cast<USWormholeWidget>(TheManager->SwitchReturn())->DeactivateButton(WormholeLane);
		}
	}
	
}