// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerPawn.h"

//ENGINE HEADERS
#include "DrawDebugHelpers.h"
#include "Math/Vector.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "SSpace.h"

//COMPONENT HEADERS
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASPlayerPawn::ASPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create the static mesh comp and set it as the root component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	//spring arm for the camera attachment
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = false;
	SpringArm->TargetArmLength = SpringArmLength;
	SpringArm->bDoCollisionTest = false;

	//create the camera comp and attach it to the static mesh
	TestCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TestCameraComp"));
	TestCameraComp->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);

	// Orbit Defaults
	bIsOrbiting = true;
	OrbitAxis = FVector(0,0,1);

	bReplicates = true;
}

void ASPlayerPawn::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerPawn, CurrentSpace);
	DOREPLIFETIME(ASPlayerPawn, bIsMoving);
	DOREPLIFETIME(ASPlayerPawn, Moves);
	DOREPLIFETIME(ASPlayerPawn, TransitionPoint);
	DOREPLIFETIME(ASPlayerPawn, bIsCentripetal);
	DOREPLIFETIME(ASPlayerPawn, bIsOrbiting);
	DOREPLIFETIME(ASPlayerPawn, SpaceCenter);

	// Rep Notify Variables
	DOREPLIFETIME(ASPlayerPawn, PawnColor);
}

// Called when the game starts or when spawned
void ASPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	//set the starting orbit speed based on the default
	CurrentOrbitSpeed = DefaultOrbitSpeed;
}

// Called every frame
void ASPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If int Moves != 0
	if (bIsMoving)
	{
		Move();
	}
	// If orbitting a space (bIsMoving can be true at the same time)
	if (bIsOrbiting)
	{
		TestCameraComp->SetRelativeRotation(FRotator(0.0f, 0.0f, GetActorRotation().Yaw + 190.0f));

		Orbit();
	}
}

void ASPlayerPawn::Orbit()
{
	if (CurrentSpace != nullptr)
	{
		SpaceCenter = CurrentSpace->GetActorLocation();
		//OldSpaceCenter = SpaceCenter;
	}

	// Moves actor by rotating vector by angle degrees around the OrbitAxis
	FVector MoveVector = GetActorLocation() - SpaceCenter;
	SetActorLocation(MoveVector.RotateAngleAxis(CurrentOrbitSpeed * GetWorld()->DeltaTimeSeconds, OrbitAxis) + SpaceCenter);
	//SetActorLocation(MoveVector.RotateAngleAxis(CurrentOrbitSpeed * GetWorld()->DeltaTimeSeconds, OrbitAxis));

	// Rotates actor to face the direction it is currently moving
	// NOTE: Currently only accomodates horizontal orbits
	FVector RotateVector = FVector::CrossProduct(GetActorLocation() - SpaceCenter, OrbitAxis);
	RotateVector.Normalize();
	SetActorRotation(FRotationMatrix::MakeFromX(RotateVector).Rotator());

	FRotator OrbitRotation = GetActorRotation().Add(0, 90, 0);
	SetActorRotation(OrbitRotation);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, RotateVector.ToString());

	// If linked to an object that is also moving, still maintains orbit distance
	//OldSpaceCenter = SpaceCenter;

	// Debug
	DrawDebugLine(GetWorld(), GetActorLocation(), SpaceCenter * 1500, FColor::Orange, false, 0.0f, 0, 5.0f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + OrbitAxis * 1500, FColor::Purple, false, 0.0f, 0, 5.0f);
	DrawDebugLine(GetWorld(), GetActorLocation(), RotateVector * 1500, FColor::Green, false, 0.0f, 0, 5.0f);
	//DrawDebugLine(GetWorld(), GetActorLocation(), MoveVector, FColor::Blue, false, 0.0f, 0, 5.0f);
}

void ASPlayerPawn::Move()
{
	ASSpace* ThisSpace = Cast<ASSpace>(CurrentSpace);

	if (ThisSpace)
	{
		// If moving TOWARDS center of board, else AWAY
		if (bIsCentripetal)
		{
			TransitionPoint = ThisSpace->GetTransitionPointA();
		}
		else
		{
			TransitionPoint = ThisSpace->GetTransitionPointB();
		}

		float DistanceToTransitionPoint = (GetActorLocation() - TransitionPoint).Size();

		GEngine->AddOnScreenDebugMessage(1, 20.0f, FColor::Red, "Distance : " + FString::SanitizeFloat(DistanceToTransitionPoint));

		// Once Transition Point set, distance toggles bIsOrbiting
		if (DistanceToTransitionPoint > RequiredDistanceToTransitionPoint)
		{
			// If deorbiting current space, else flying through space to next... space
			if (bIsOrbiting)
			{
				CurrentOrbitSpeed = DeOrbitSpeed;
			}
			else
			{
				SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TransitionPoint, GetWorld()->DeltaTimeSeconds, TransitionSpeed));
				FVector RotateVector = FVector(GetActorLocation() - TransitionPoint);
				RotateVector.Normalize();
				SetActorRotation(FRotationMatrix::MakeFromX(RotateVector).Rotator());

				FRotator MovingRotation = GetActorRotation().Add(0, 90, 0);
				SetActorRotation(MovingRotation);
			}
		}
		else
		{
			// Setting Transition when moving forward/backward
			if (bIsOrbiting)
			{
				if (bIsCentripetal)
				{
					CurrentSpace = ThisSpace->GetNextSpace();
				}
				else
				{
					CurrentSpace = ThisSpace->GetPreviousSpace();
				}

				bIsOrbiting = false;
			}
			else
			{
				// Has reached next Transition Point, decrement Moves and proceed appropriately
				Moves--;

				if (Moves > 0)
				{
					if (bIsCentripetal)
					{
						CurrentSpace = ThisSpace->GetNextSpace();
					}
					else
					{
						CurrentSpace = ThisSpace->GetPreviousSpace();
					}
				}
				else
				{
					GetPlayerState<ASPlayerState>()->SetCurrentSpaceNum(Cast<ASSpace>(CurrentSpace)->ReturnSpaceNum());

					TransitionPoint = FVector(0, 0, 0);

					bIsMoving = false;
					bIsOrbiting = true;

					CurrentOrbitSpeed = DefaultOrbitSpeed;

					ThisSpace->SetOrbitingPlayer(this);
				}
			}
		}
	}
}

void ASPlayerPawn::SetPawnColor(FLinearColor ThisPawnColor)
{
	// Change player pawn color
	if (HasAuthority())
	{
		// Rep_Notify
		PawnColor = ThisPawnColor;

		NeonDynamicMaterialInst = UMaterialInstanceDynamic::Create(MeshComp->GetMaterial(0), MeshComp);
		AluminiumDynamicMaterialInst = UMaterialInstanceDynamic::Create(MeshComp->GetMaterial(3), MeshComp);

		NeonDynamicMaterialInst->SetVectorParameterValue(FName(TEXT("Color")), PawnColor);
		MeshComp->SetMaterial(0, NeonDynamicMaterialInst);

		AluminiumDynamicMaterialInst->SetVectorParameterValue(FName(TEXT("Color")), PawnColor);
		MeshComp->SetMaterial(3, AluminiumDynamicMaterialInst);
	}
}

//move the ship forward one space each time this function is called
void ASPlayerPawn::BeginMoveForward()
{	
	Moves++;

	bIsCentripetal = true;

	bIsMoving = true;
}

//move the ship backward one space each time this function is called
void ASPlayerPawn::BeginMoveBackward()
{
	Moves++;

	bIsCentripetal = false;

	bIsMoving = true;
}

// Called to bind functionality to input
void ASPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("NextSpace", IE_Pressed, this, &ASPlayerPawn::BeginMoveForward);
	PlayerInputComponent->BindAction("PreviousSpace", IE_Pressed, this, &ASPlayerPawn::BeginMoveBackward);
}

// MULTIPLAYER FUNCTIONALITY

//on the change of color variable, replicate the update of the materials of the ship
void ASPlayerPawn::OnRep_Color()
{
	NeonDynamicMaterialInst = UMaterialInstanceDynamic::Create(MeshComp->GetMaterial(0), MeshComp);
	AluminiumDynamicMaterialInst = UMaterialInstanceDynamic::Create(MeshComp->GetMaterial(3), MeshComp);

	NeonDynamicMaterialInst->SetVectorParameterValue(FName(TEXT("Color")), PawnColor);
	MeshComp->SetMaterial(0, NeonDynamicMaterialInst);

	AluminiumDynamicMaterialInst->SetVectorParameterValue(FName(TEXT("Color")), PawnColor);
	MeshComp->SetMaterial(3, AluminiumDynamicMaterialInst);
}


