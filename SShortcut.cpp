// Fill out your copyright notice in the Description page of Project Settings.

#include "SShortcut.h"

//CLASS HEADERS
#include "SPlayerPawn.h"

ASShortcut::ASShortcut()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASShortcut::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		Shortcut();
		bIsActive = false;
	}
}

//update the variables of the shortcut destination space and reset the variables of this space
void ASShortcut::Shortcut()
{
	if (OrbitingPlayer)
	{
		if (ShorcutDestination)
		{
			ASPlayerPawn* ShortcuttingPlayer = Cast<ASPlayerPawn>(OrbitingPlayer);
			ASSpace* ShortcutEnd = Cast<ASSpace>(ShorcutDestination);

			OrbitingPlayer->SetActorLocation(ShortcutEnd->GetTransitionPointA());
			ShortcuttingPlayer->SetCurrentSpace(ShorcutDestination);
			ShortcutEnd->SetOrbitingPlayer(OrbitingPlayer);
			OrbitingPlayer = nullptr;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, "Shorcut Destination not set");
		}
	}
}