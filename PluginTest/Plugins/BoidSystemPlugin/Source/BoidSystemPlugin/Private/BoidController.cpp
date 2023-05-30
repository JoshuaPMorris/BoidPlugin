// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidController.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"

ABoidController::ABoidController()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
}

ABoidController::~ABoidController()
{
	Boids.Empty();
	CachedBoids.Empty();
	BoidRules.Empty();

	this->Destroy();
}

void ABoidController::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Look! A BoidController..."));
	
	SetShowMouseCursor(true);

	for (int i = 1; i <= StartingBoids; i++)
	{
		ABoid* Boid = GetWorld()->SpawnActor<ABoid>(ABoid::StaticClass(), FVector(FMath::RandRange(WallArea.X - WallArea.X, WallArea.X),
			FMath::RandRange(WallArea.Y - WallArea.Y, WallArea.Y), 1), Rot);
		RandomizeBoidVelocity(Boid);
		Boid->SetVisualRange(VisualRange);
		Boid->SetSpeed(Speed);
		Boid->SetActorRotation(FRotator(0));

		CachedBoids.Emplace(Boid);
	}
}

void ABoidController::InitializeRules()
{
	BoidRules.Empty();
	
	BoidRules.Emplace(MakeUnique<CohesionRule>(CohesionWeight));
	BoidRules.Emplace(MakeUnique<SeparationRule>(SeparationWeight));
	BoidRules.Emplace(MakeUnique<AlignmentRule>(AlignmentWeight));
	BoidRules.Emplace(MakeUnique<PointRepulsionRule>(PointWeight, true, false, this,
		this, DebugLines, EnableMouse, LeftClick, RightClick));
	BoidRules.Emplace(MakeUnique<BoundedAreaRule>(WallArea.Y, WallArea.X, 10, WallWeight, IsBounded));
	
	DefaultWeights = new float[BoidRules.Num()];
	int i = 0;
	for (const auto& Rule : BoidRules)
	{
		DefaultWeights[i++] = Rule->Weight;
	}
}

void ABoidController::ApplyBoidRules()
{
	for (const auto& Boid : CachedBoids)
	{
		Boid->SetBoidRules(BoidRules);
	}
}

void ABoidController::ApplyOneBoidRules(ABoid* Boid)
{
	Boid->SetBoidRules(BoidRules);
}

void ABoidController::RandomizeBoidVelocity(ABoid* Boid)
{
	Boid->SetVelocity2D(FVector2D(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100)) * Speed);
}

void ABoidController::LeftMouse()
{
	if (!LeftClick)
		LeftClick = true;
	else
	{
		LeftClick = false;
	}
}

void ABoidController::RightMouse()
{
	if (!RightClick)
		RightClick = true;
	else
	{
		RightClick = false;
	}
}

void ABoidController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetShowMouseCursor(true);

	MousePos = Hit.Location;

	InitializeRules();
	ApplyBoidRules();
}

void ABoidController::SpawnBoid()
{
	bool bHit = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
	
	FVector SpawnSpot(Hit.Location.X, Hit.Location.Y, 1);
	
	if (bHit)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Hit! ..."));
		UE_LOG(LogTemp, Warning, TEXT("MouseToWorld Location: %f, %f"), Hit.Location.X, Hit.Location.Y);
		
		DrawDebugBox(GetWorld(), SpawnSpot, FVector(1), FColor::Orange, false, 10.0f);

		// Create the Boid.
		ABoid* Boid = GetWorld()->SpawnActor<ABoid>(ABoid::StaticClass(), SpawnSpot, Rot);
		RandomizeBoidVelocity(Boid);
		Boid->SetVisualRange(VisualRange);
		Boid->SetSpeed(Speed);
		ApplyOneBoidRules(Boid);
		Boid->SetActorRotation(FRotator(0));

		CachedBoids.Emplace(Boid);
	}
}

void ABoidController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Setup game input bindings here.
	InputComponent->BindAction("SpawnBoid", IE_Pressed, this, &ABoidController::SpawnBoid);
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &ABoidController::LeftMouse);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &ABoidController::RightMouse);
	InputComponent->BindAction("LeftClick", IE_Released, this, &ABoidController::LeftMouse);
	InputComponent->BindAction("RightClick", IE_Released, this, &ABoidController::RightMouse);
}