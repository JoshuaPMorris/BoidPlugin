// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCamera.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerCamera::APlayerCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Create our components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	StaticMeshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	//Attach our components
	StaticMeshComp->SetupAttachment(RootComponent);
	SpringArmComp->SetupAttachment(StaticMeshComp);
	CameraComp->SetupAttachment(SpringArmComp,USpringArmComponent::SocketName);
	// 185.25 393 398 // 175 365 -0.5, 0.375 0.8 1  // 370 790
	//Setting Default Variables and Behavior of the SpringArmComponent
	SpringArmComp->SetRelativeLocationAndRotation(FVector(185.25f, 393.0f, 398.0f), FRotator(-90.0f, 0.0f, 0.0f));
	SpringArmComp->TargetArmLength = 0.f;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 3.0f;

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

APlayerCamera::~APlayerCamera()
{
	this->Destroy();
}

// Called when the game starts or when spawned
void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();

	//check(GEngine != nullptr);
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Look! A BoidBrain..."));

	this->SetActorRotation(FRotator(0, 0, 0));
}

// Called every frame
void APlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle movement based on our "MoveX" and "MoveY" axes
	{
		if (!MovementInput.IsZero())
		{
			//Scale our movement input axis values by 100 units per second
			MovementInput = MovementInput.GetSafeNormal() * 100.0f;
			FVector NewLocation = GetActorLocation();
			NewLocation += GetActorForwardVector() * MovementInput.X * DeltaTime;
			NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
			SetActorLocation(NewLocation);
		}
	}
}

// Called to bind functionality to input
void APlayerCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

