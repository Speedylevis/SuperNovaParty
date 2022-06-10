// Fill out your copyright notice in the Description page of Project Settings.


#include "SScoreWidget.h"

//COMPONENT HEADERS
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

//update the color of each player's score text
void USScoreWidget::UpdatePlayerColors(TArray<FLinearColor> WColors)
{
	Score_P1->SetColorAndOpacity(WColors[0]);
	Score_P2->SetColorAndOpacity(WColors[1]);
	Score_P3->SetColorAndOpacity(WColors[2]);
	Score_P4->SetColorAndOpacity(WColors[3]);
}

//update the text to represent each player's score
void USScoreWidget::UpdatePlayersScores(TArray<float> PScores)
{
	Score_P1->SetText(FText::AsNumber(PScores[0]));
	Score_P2->SetText(FText::AsNumber(PScores[1]));
	Score_P3->SetText(FText::AsNumber(PScores[2]));
	Score_P4->SetText(FText::AsNumber(PScores[3]));
}

//update the icon's color and opacity to represent the location of offscreen players
void USScoreWidget::UpdateLocationIndicators(FLinearColor ColorToUpdate, FVector2D LocationToUpdate, bool OnScreen)
{
	if (ColorToUpdate == FLinearColor::Blue)
	{
		if (OnScreen)
		{ Image_B->SetOpacity(0.0f); }
		else
		{ Image_B->SetOpacity(1.0f); }

		Cast<UCanvasPanelSlot>(Image_B->Slot)->SetPosition(LocationToUpdate);
	}
	else if (ColorToUpdate == FLinearColor::Green)
	{
		if (OnScreen)
		{ Image_G->SetOpacity(0.0f); }
		else
		{ Image_G->SetOpacity(1.0f); }

		Cast<UCanvasPanelSlot>(Image_G->Slot)->SetPosition(LocationToUpdate);
	}
	else if (ColorToUpdate == FLinearColor::Red)
	{
		if (OnScreen)
		{ Image_R->SetOpacity(0.0f); }
		else
		{ Image_R->SetOpacity(1.0f); }

		Cast<UCanvasPanelSlot>(Image_R->Slot)->SetPosition(LocationToUpdate);
	}
	else if (ColorToUpdate == FLinearColor::Yellow)
	{
		if (OnScreen)
		{ Image_Y->SetOpacity(0.0f); }
		else
		{ Image_Y->SetOpacity(1.0f); }

		Cast<UCanvasPanelSlot>(Image_Y->Slot)->SetPosition(LocationToUpdate);
	}
	
}