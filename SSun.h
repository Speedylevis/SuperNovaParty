// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSun.generated.h"

UCLASS()
class COLLABTUT_API ASSun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASSun();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//when a player score changes, check to see if they pass the current highest point count
	void UpdateMaxPodCount(float NewPlayerScore);

	//if max pod count is updated, check to see if it passes the threshold to move the sun to the next phase
	UFUNCTION(BlueprintCallable)
	void UpdatePodCounter();

	//cast from a MultiCast function to have server/clients all scale the sun
	void UpdateSunWithGS();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	//the highest score any player has reached in the game
	UPROPERTY(Replicated, EditAnywhere, Category = "SunPhase")
	int MaxPodCount = 0;

	//the sun has 5 phases, at phase 6 the game ends
	UPROPERTY(Replicated)
	int CurrentSunPhase = 0;

	//Scene components
	class UStaticMeshComponent* StaticMeshComp;
	class URotatingMovementComponent* RotMoveComp;
	class UParticleSystem* SunPE;

	//sun flare particles for each face of the sun
	UParticleSystemComponent* SunFace_Top;
	UParticleSystemComponent* SunFace_Bottom;
	UParticleSystemComponent* SunFace_Left;
	UParticleSystemComponent* SunFace_Right;
	UParticleSystemComponent* SunFace_Back;

	UStaticMesh* SunMesh;

	//tells the sun to move to the next phase
	bool ScaleUp;
	//tells the sun that a player has reached a score to end the game
	bool ActivateImplosion;

	//Tessalation
	UPROPERTY(EditAnywhere, Category = "Implosion\|Tessalation")
	float TessalationDestination;
	UPROPERTY(EditAnywhere, Category = "Implosion\|Tessalation")
	float TessalationSpeed;

	//Color
	UPROPERTY(Replicated)
	int ColorToPick = 0;

	UPROPERTY(EditAnywhere, Category = "Implosion\|Color")
	float ColorSpeed;

	FLinearColor OriginalColor;
	UPROPERTY(EditAnywhere, Category = "Implosion\|Color")
	FLinearColor ColorDestination_1 = FLinearColor(1.0f, 0.31f, 0.0f, 1.0f);
	UPROPERTY(EditAnywhere, Category = "Implosion\|Color")
	FLinearColor ColorDestination_2 = FLinearColor(1.0f, 0.88f, 0.0f, 1.0f);
	UPROPERTY(EditAnywhere, Category = "Implosion\|Color")
	FLinearColor ColorDestination_3 = FLinearColor(0.07f, 0.83f, 0.0f, 1.0f);
	UPROPERTY(EditAnywhere, Category = "Implosion\|Color")
	FLinearColor ColorDestination_4 = FLinearColor(0.0f, 0.47f, 1.0f, 1.0f);
	UPROPERTY(EditAnywhere, Category = "Implosion\|Color")
	FLinearColor ColorDestination_5 = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//Scale
	UPROPERTY(EditAnywhere, Category = "Implosion\|Scale")
	float ScaleSpeed;
	UPROPERTY(EditAnywhere, Category = "Implosion\|Scale")
	float ScaleComparison;
	UPROPERTY(EditAnywhere, Category = "Implosion\|Scale")
	FVector ScaleDestination;
	FVector OriginalScale;

	//Implosion
	UPROPERTY(EditAnywhere, Category = "Implosion")
	float ImplodeSpeed;

	//Materials
	UPROPERTY(Replicated)
	UMaterialInstanceDynamic* SunMaterial;
	UPROPERTY(Replicated)
	UMaterialInstanceDynamic* SunPEMaterial;
};
