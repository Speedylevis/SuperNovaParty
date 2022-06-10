// Fill out your copyright notice in the Description page of Project Settings.


#include "SSun.h"

//ENGINE HEADERS
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

//CLASS HEADERS
#include "SBoardGS.h"
#include "SPlayerController.h"
#include "SSunSpace.h"

// Sets default values
ASSun::ASSun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create scene comps and store references
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComp;

	RotMoveComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
	RotMoveComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);

	ConstructorHelpers::FObjectFinder<UStaticMesh> SunMeshClass(TEXT("/Game/Meshes/NovaHead/NovaHead_Export"));
	SunMesh = SunMeshClass.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> SunPEClass(TEXT("/Game/ParticleEffects/SunFaces"));
	SunPE = SunPEClass.Object;
}

void ASSun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	 
	DOREPLIFETIME(ASSun, SunMaterial);
	DOREPLIFETIME(ASSun, SunPEMaterial);
	DOREPLIFETIME(ASSun, ColorToPick);
	DOREPLIFETIME(ASSun, MaxPodCount);
}

// Called when the game starts or when spawned
void ASSun::BeginPlay()
{
	Super::BeginPlay();

	//store original values of variables for reference
	OriginalScale = GetActorScale3D();
	StaticMeshComp->SetStaticMesh(SunMesh);
	
	SunMaterial = UMaterialInstanceDynamic::Create(StaticMeshComp->GetMaterial(1), StaticMeshComp);
	SunMaterial->GetVectorParameterValue(FName(TEXT("Sun Color")), OriginalColor);

	ScaleDestination = GetActorScale3D() + FVector(1.0f, 1.0f, 1.0f);

	//spawn the particle effects for each face of the sun
	SunFace_Top = UGameplayStatics::SpawnEmitterAttached(SunPE, StaticMeshComp, FName("Socket_Top"));

	SunFace_Bottom = UGameplayStatics::SpawnEmitterAttached(SunPE, StaticMeshComp, FName("Socket_Bottom"));
	SunFace_Bottom->SetRelativeRotation(FRotator(0.0f, 0.0f, 180.0f));

	SunFace_Left = UGameplayStatics::SpawnEmitterAttached(SunPE, StaticMeshComp, FName("Socket_Left"));
	SunFace_Left->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));

	SunFace_Right = UGameplayStatics::SpawnEmitterAttached(SunPE, StaticMeshComp, FName("Socket_Right"));
	SunFace_Right->SetRelativeRotation(FRotator(0.0f, 0.0f, 270.0f));

	SunFace_Back = UGameplayStatics::SpawnEmitterAttached(SunPE, StaticMeshComp, FName("Socket_Back"));
	SunFace_Back->SetRelativeRotation(FRotator(270.0f, 0.0f, 0.0f));

	SunPEMaterial = UMaterialInstanceDynamic::Create(SunFace_Top->GetMaterial(0), SunFace_Top);
}

// Called every frame
void ASSun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ScaleUp)
	{
		SetActorScale3D(FMath::Lerp(GetActorScale3D(), ScaleDestination, ScaleSpeed * DeltaTime));

		//once actor finishes scaling, prep the sun for its next phase
		if ((GetActorScale3D() - ScaleDestination).Size() <= ScaleComparison)
		{
			++ColorToPick;

			GEngine->AddOnScreenDebugMessage(318, 20.0f, FColor::Blue, "Done Scaling");

			ScaleUp = false;

			ScaleDestination += FVector(1.0f, 1.0f, 1.0f);

			++CurrentSunPhase;

			if (CurrentSunPhase == 6)
			{ ActivateImplosion = true; }
		}
		//scale up sun, moving its tessalation and color to the next phase
		else
		{
			float CurrentTessalation;
			SunMaterial->GetScalarParameterValue(FName(TEXT("Tessalation Multiplier")), CurrentTessalation);
			SunMaterial->SetScalarParameterValue(FName(TEXT("Tessalation Multiplier")), FMath::Lerp(CurrentTessalation, TessalationDestination, TessalationSpeed * DeltaTime));

			if (ColorToPick == 0)
			{
				FLinearColor CurrentColor;
				SunMaterial->GetVectorParameterValue(FName(TEXT("Sun Color")), CurrentColor);
				SunMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_1, ColorSpeed * DeltaTime));
				SunPEMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_1, ColorSpeed * DeltaTime));
			}
			else if (ColorToPick == 1)
			{
				FLinearColor CurrentColor;
				SunMaterial->GetVectorParameterValue(FName(TEXT("Sun Color")), CurrentColor);
				SunMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_2, ColorSpeed * DeltaTime));
				SunPEMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_2, ColorSpeed * DeltaTime));
			}
			else if (ColorToPick == 2)
			{
				FLinearColor CurrentColor;
				SunMaterial->GetVectorParameterValue(FName(TEXT("Sun Color")), CurrentColor);
				SunMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_3, ColorSpeed * DeltaTime));
				SunPEMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_3, ColorSpeed * DeltaTime));
			}
			else if (ColorToPick == 3)
			{
				FLinearColor CurrentColor;
				SunMaterial->GetVectorParameterValue(FName(TEXT("Sun Color")), CurrentColor);
				SunMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_4, ColorSpeed * DeltaTime));
				SunPEMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_4, ColorSpeed * DeltaTime));
			}
			else if (ColorToPick == 4)
			{
				FLinearColor CurrentColor;
				SunMaterial->GetVectorParameterValue(FName(TEXT("Sun Color")), CurrentColor);
				SunMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_5, ColorSpeed * DeltaTime));
				SunPEMaterial->SetVectorParameterValue(FName(TEXT("Sun Color")), FMath::Lerp(CurrentColor, ColorDestination_5, ColorSpeed * DeltaTime));
			}

			StaticMeshComp->SetMaterial(1, SunMaterial);

			SunFace_Top->SetMaterial(0, SunPEMaterial);
			SunFace_Bottom->SetMaterial(0, SunPEMaterial);
			SunFace_Left->SetMaterial(0, SunPEMaterial);
			SunFace_Right->SetMaterial(0, SunPEMaterial);
			SunFace_Back->SetMaterial(0, SunPEMaterial);
		}
	}

	//when a character wins the game, implode the sun and move to the win screen
	if (ActivateImplosion)
	{
		SetActorScale3D(FMath::Lerp(GetActorScale3D(), FVector(0.0f, 0.0f, 0.0f), ImplodeSpeed * DeltaTime));

		if ((FVector(0.0f, 0.0f, 0.0f) - GetActorScale3D()).Size() <= ScaleComparison)
		{
			//SpawnExplosionEmitter here

			if (GetWorld()->IsServer())
			{ Cast<ASBoardGS>(GetWorld()->GetGameState())->InitiateWinScreen(); }

			Destroy();
		}
	}
}

//store the threshold for the maximum number of score a player has reached
void ASSun::UpdateMaxPodCount(float NewPlayerScore)
{
	if (NewPlayerScore > MaxPodCount)
	{
		MaxPodCount = NewPlayerScore;
		UpdatePodCounter();
	}
}

//check if the max pod count has reached the threshold to move to the next phase
void ASSun::UpdatePodCounter()
{
	if (MaxPodCount >= 100 && CurrentSunPhase == 0)
	{ Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->SunScalePassToGS(); }
	else if (MaxPodCount >= 200 && CurrentSunPhase == 1)
	{ Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->SunScalePassToGS(); }
	else if (MaxPodCount >= 300 && CurrentSunPhase == 2)
	{ Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->SunScalePassToGS(); }
	else if (MaxPodCount >= 400 && CurrentSunPhase == 3)
	{ Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->SunScalePassToGS(); }
	else if (MaxPodCount >= 500 && CurrentSunPhase == 4)
	{ Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->SunScalePassToGS(); }
	else if (MaxPodCount >= 600 && CurrentSunPhase == 5)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASSunSpace::StaticClass(), FoundActors);
		Cast<ASBoardGS>(GetWorld()->GetGameState())->FocusCameraOnActor(FoundActors[0], 0.5f);

		Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController())->SunScalePassToGS();
	}
}

//called from MultiCast function for all clients to scale the sun to the next phase
void ASSun::UpdateSunWithGS()
{
	ScaleUp = true;
}