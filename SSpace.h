// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSpace.generated.h"

UCLASS()
class COLLABTUT_API ASSpace : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASSpace();

	//set which player is orbiting this space
	void SetOrbitingPlayer(APawn* ThisOrbitingPlayer);

	//access next and previous space (set in level instance)
	AActor* GetPreviousSpace() { return PreviousSpace; };
	AActor* GetNextSpace() { return NextSpace; };

	//access the location of the transition points orbiting a space
	FVector GetTransitionPointA() { return GetTransform().TransformPosition(TransitionPointA); };
	FVector GetTransitionPointB() { return GetTransform().TransformPosition(TransitionPointB); };
	FVector GetTransitionPointC() { return GetTransform().TransformPosition(TransitionPointC); };
	FVector GetTransitionPointD() { return GetTransform().TransformPosition(TransitionPointD); };

	//access the space number and lane number of this space (manuallly assigned in level instance)
	int ReturnSpaceNum() { return SpaceNum; };
	int ReturnLaneNum() { return LaneNum; };

	//allows the next space to update based on lane choice at sun space
	void SetNextSpace(AActor* ChosenSpace) { NextSpace = ChosenSpace; };
	
	//move the player to the first dump space
	void DumpPlayer();

	//sets the pawns current space to this actor and udpates the pawn count
	void AssignPlayerToSpace(APawn* PawnToSet);

	//setters and getters to update/return the pawn count from other classes
	void SetPawnCount(int NewPawnCount) { PawnCount = NewPawnCount; };
	int ReturnPawnCount() { return PawnCount; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//bool to trigger a space's specific method (used in tick of derived class)
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Space")
	bool bIsActive;

	//the player that is orbiting this space
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Space")
	APawn* OrbitingPlayer;

	//allows a delay before moving the next player's turn
	void NextTurn();

	UPROPERTY(VisibleDefaultsOnly, Category = "Space\|Components")
	UStaticMeshComponent* StaticMeshComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	//points to lock player orbit positions for landing/leaving a space
	UPROPERTY(Meta = (MakeEditWidget = true))
	FVector TransitionPointA;

	UPROPERTY(Meta = (MakeEditWidget = true))
	FVector TransitionPointB;

	FVector TransitionPointC;

	FVector TransitionPointD;

	//the spaces that a player will transition to when leaving a space
	UPROPERTY(EditInstanceOnly, Category = "Space")
	AActor* PreviousSpace;

	UPROPERTY(EditInstanceOnly, Category = "Space")
	AActor* NextSpace;

	//bools to check if the space is the initial player space for lane choice
	UPROPERTY(EditInstanceOnly, Category = "Board")
	bool bIsStartingSpace;

	UPROPERTY(EditInstanceOnly, Category = "Board")
	bool bIsLaneGate = false;

	bool bStartSpaceSet;

	//number of pawns that are at this space
	int PawnCount;

	bool bDumpSpaceSet;

	//the first space in the dump to move players to when being sent to the dump
	UPROPERTY(EditInstanceOnly, Category = "Board")
	AActor* DumpingSpace;
	
	//bool to prevent players from being sent to the dump under conditions where they normally would
	UPROPERTY(EditInstanceOnly, Category = "Board")
	bool bNoDumpingAllowed;

	//Scene Components for the space
	UPROPERTY(VisibleDefaultsOnly, Category = "Space\|Components")
	USceneComponent* SceneComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Space\|Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Space\|Components")
	float SpringArmLength = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "Space\|Components")
	class UCameraComponent* TestCameraComp;

	//properties to identify the space based on its instance and lane
	UPROPERTY(EditAnywhere, Category = "Position")
	int LaneNum;

	UPROPERTY(EditInstanceOnly, Category = "Position")
	int SpaceNum;
};
