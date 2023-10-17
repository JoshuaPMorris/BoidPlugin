// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"

#include "Components/BoxComponent.h"

//BoidSettings Settings;

// Sets default values
ABoid::ABoid()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root comp for physics.
	UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = BoxComponent;
	BoxComponent->InitBoxExtent(FVector(1.0f, 1.0f, 1.0f));
	BoxComponent->SetCollisionProfileName(TEXT("Boid"));

	// Creating mesh comp to see.
	UStaticMeshComponent* BoxVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	BoxVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxVisualAsset(TEXT("/BoidSystemPlugin/Shapes/Shape_Cube"));

	if (BoxVisualAsset.Succeeded())
	{
		BoxVisual->SetStaticMesh(BoxVisualAsset.Object);
		BoxVisual->SetRelativeLocation(FVector(0.0f));
		BoxVisual->SetWorldScale3D(FVector(0.01f));
	}
}

ABoid::~ABoid()
{
	//ListOfBoidsInVision.Empty();
	ListOfBoidsInVision.Reset();
	this->Destroy();
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	
	ListOfBoidsInVision.Empty();

	//if (Settings.ListOfBoids.Num() == 0)
	//{
	//	Settings.ListOfBoids.Empty();
	//}
	//Settings.ListOfBoids.Add(this);

	SetIfConstantSpeed(false);
	SetMaxAcceleration(100);
	SetSpeed(300);
}

void ABoid::ComputeNeighbourhood()
{
	for (int i = 0; i < Settings.ListOfBoids.Num(); i++)
	{
		// Check if the Boid being looked close enough.
		if (Settings.ListOfBoids[i] != this)
		{
			if (GetDistanceTo(Settings.ListOfBoids[i]) <= VisualRange)
			{
				// Check if vision list is empty, if so then add the Boid.
				if (ListOfBoidsInVision.Num() == 0)
				{
					ListOfBoidsInVision.Add(Settings.ListOfBoids[i]);
					break;
				}
			
				// Check if the Boid is in the vision already.
				for (int j = 0; j < ListOfBoidsInVision.Num(); j++)
				{
					if (ListOfBoidsInVision[j] == Settings.ListOfBoids[i])
					{
						break;
					}
					if (j == ListOfBoidsInVision.Num() - 1 && ListOfBoidsInVision[j] != Settings.ListOfBoids[i])
					{
						ListOfBoidsInVision.Add(Settings.ListOfBoids[i]);
					}
				}
			}
		}
	}

	// Check vision list for boids not in range.
	for (int i = 0; i < ListOfBoidsInVision.Num(); i++)
	{
		if (ListOfBoidsInVision[i] != this)
		{
			// If not in range
			if (GetDistanceTo(ListOfBoidsInVision[i]) > VisualRange)
			{
				// Remove by swap, shrink list by 1.
				ListOfBoidsInVision.RemoveAtSwap(i, 1, true);
			}
		}
	}
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ComputeNeighbourhood();

	Position = FVector2D(GetActorLocation());

	
	for (auto& Rule : Rules)
	{
		FVector2D WeightedForce = Rule->ComputeWeightedForce(ListOfBoidsInVision, this);
		ApplyForce(WeightedForce);
	}

	if (GetAcceleration().Size() > GetMaxAcceleration())
	{
		FVector2D Temp = GetAcceleration();
		Temp.Normalize();
		SetAcceleration(Temp * GetMaxAcceleration());
	}
	
	SetVelocity2D(Velocity + Acceleration);

	ResetAcceleration();

	if (GetIfConstantSpeed())
	{
		FVector2D Temp = GetVelocity2D();
		Temp.Normalize();
		SetVelocity2D(Temp * GetSpeed());
	}
	else if (GetVelocity2D().Size() > GetSpeed())
	{
		FVector2D Temp = GetVelocity2D();
		Temp.Normalize();
		SetVelocity2D(Temp * GetSpeed());
	}
	
	this->SetPosition(FVector2D(GetPosition().X + (Velocity.X * DeltaTime), GetPosition().Y + (Velocity.Y * DeltaTime)));
	BoidRotation();
}

void ABoid::ApplyForce(FVector2D Force)
{
	SetAcceleration(GetAcceleration() + Force);
}

void ABoid::BoidRotation()
{
	float DirectionAngle = FMath::Atan2(GetVelocity2D().Y, GetVelocity2D().X);
	FRotator YawRotation = FRotator(0.0f, FMath::RadiansToDegrees(DirectionAngle), 0.0f);
	float PitchAngle = FMath::Atan2(GetVelocity2D().Y, FVector::Dist(FVector(0.0f, 0.0f, 0.0f), FVector(GetVelocity2D().X, GetVelocity2D().Y, 0.0f)));
	FRotator PitchRotation = FRotator(FMath::RadiansToDegrees(PitchAngle), 0.0f, 0.0f);
	FRotator FinalRotation = YawRotation + PitchRotation;
	FinalRotation.Pitch = 0;
	FinalRotation.Roll = 0;
	this->SetRotation(FinalRotation);
}

void ABoid::ResetAcceleration()
{
	Acceleration = FVector2D().ZeroVector;
}