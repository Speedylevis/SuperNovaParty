// Fill out your copyright notice in the Description page of Project Settings.


#include "SGamble.h"

//ENGINE HEADERS
#include "Blueprint/WidgetBlueprintLibrary.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "WidgetManager.h"

ASGamble::ASGamble()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASGamble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		GambleWidget();
		bIsActive = false;
	}

}

//activate the gamble space widget
void ASGamble::GambleWidget()
{
	int CurrentTurn = GetWorld()->GetGameState<ASBoardGS>()->ReturnCurrentPlayerTurn();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		//grab the controller and cast it to the custom PC
		APlayerController* PC = It->Get();
		ASPlayerController* ThisPC = Cast<ASPlayerController>(PC);

		FString ThisName = ThisPC->GetPlayerState<ASPlayerState>()->GetPlayerName();

		if (CurrentTurn == 1 && ThisName == "P1" ||
			CurrentTurn == 2 && ThisName == "P2" ||
			CurrentTurn == 3 && ThisName == "P3" ||
			CurrentTurn == 4 && ThisName == "P4")
		{
			ThisPC->SwitchInteractability(true);
		}
	}

	//look for all widgets on screen, by using a manager with a widget switcher we will only have one widget on scree and so we are able to use this function
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());

	//access the current widget on screen, the ingame widget
	UWidgetManager* TheManager = Cast<UWidgetManager>(FoundWidgets[0]);

	TheManager->SwitchWidgets(2);
}