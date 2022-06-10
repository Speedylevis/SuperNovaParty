// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SCardSystem.generated.h"

UCLASS()
class COLLABTUT_API ASCardSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASCardSystem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//vector for where the card moves when selected
	void SetCenterScreen(FVector Center) { CenterScreen = Center; };

	//vector for where a card moves when hovered over, but not selected
	void SetHoverLocation(FVector Hover) { HoverLocation = Hover; };

	//set the value of this card
	void SetCardString(FString StringToSet) { CardString = StringToSet; };

	//set the mesh of this card based on its string value
	void SetCardMesh();

	//getter and setter for which of the three cards is this card
	void SetCardIndex(int CIndex) { CardIndex = CIndex; };
	int ReturnCardIndex() { return CardIndex; };

	//getter and setter for the paint of one of the three cards
	void SetCardPaint(FString PaintToSet) { CardPaint = PaintToSet; };

	//bool variables updated from the GameState after NetMultiCast
	void SetHoverOff(bool WhatToSet) { HoverOff = WhatToSet; };
	void SetHoverOn(bool WhatToSet) { HoverOn = WhatToSet; };
	void SetAbleToHover(bool WhatToSet) { AbleToHover = WhatToSet; };
	void SetAdjustSpinSpeed(bool WhatToSet) { AdjustSpinSpeed = WhatToSet; };

	//bool variables updated from Paint Space
	void SetCardEffect(AActor* AffectedCard, int CardEffect);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	//Scene components
	class UStaticMeshComponent* StaticMeshComp;
	class URotatingMovementComponent* RotMoveComp;

	//card explosion particle effect
	class UParticleSystem* CardEmitter;

	//locations for the cards to move when interacted with
	FVector OriginalLocation;
	FVector CenterScreen;
	FVector HoverLocation;

	//bools to control the card movement when interacted 
	bool AbleToHover = true;
	bool HoverOn = false;
	bool HoverOff = false;
	bool AdjustSpinSpeed = false;
	bool DecrementSpinSpeed = false;
	bool SpawnOnce = true;

	//variables to control the speed of the card's movement and dissapearance
	UPROPERTY(EditAnywhere)
	float CardMoveSpeed = 40.0f;
	UPROPERTY(EditAnywhere)
	float CardRotSpeed = 90.0f;
	UPROPERTY(EditAnywhere)
	float IncrementSpeed = 600.0f;
	UPROPERTY(EditAnywhere)
	float MaxRotSpeed = 2000.0f;
	UPROPERTY(EditAnywhere)
	float TempMaterialOpacity = 1.0f;
	UPROPERTY(EditAnywhere)
	float CardDisappearSpeed = 0.4f;

	//references to the value of this card
	int MovementNumber;
	FString CardString;
	int CardIndex;
	FString CardPaint;

	//references to store the static meshes for precision cards
	UStaticMesh* CardMesh_1;
	UStaticMesh* CardMesh_2;
	UStaticMesh* CardMesh_3;
	UStaticMesh* CardMesh_4;
	UStaticMesh* CardMesh_5;
	UStaticMesh* CardMesh_6;

	//references to store the static meshes for ranged cards
	UStaticMesh* CardMesh_1T3;
	UStaticMesh* CardMesh_1T6;
	UStaticMesh* CardMesh_2T4;
	UStaticMesh* CardMesh_3T5;
	UStaticMesh* CardMesh_4T6;

	//references to the materials of this card's mesh to control opacity
	UMaterialInstanceDynamic* dynMaterial_1;
	UMaterialInstanceDynamic* dynMaterial_2;
	UMaterialInstanceDynamic* dynMaterial_3;

	//references to the materials of the card's paint
	UMaterialInstanceDynamic* PaintDynamicMaterialInst;

	//methods for the player's interactions with this card
	UFUNCTION()
	void ClickMethod(AActor* TouchedActor, FKey ButtonPressed);
	UFUNCTION()
	void HoverMethod(AActor* HoveredActor);
	UFUNCTION()
	void HoverMethod_End(AActor* HoveredActor);
};
