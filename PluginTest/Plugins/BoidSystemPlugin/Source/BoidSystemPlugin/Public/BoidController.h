// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Boid.h"
#include "FBoidRules.h"
//#include "BoidSettings.h"

#include "BoidController.generated.h"

using BoidPtr = TUniquePtr<ABoid>;

/**
 * 
 */
UCLASS()
class BOIDSYSTEMPLUGIN_API ABoidController : public APlayerController
{
	GENERATED_BODY()
public:
	ABoidController();
	virtual ~ABoidController() override;
	
	virtual void BeginPlay() override;
	
protected:
	virtual void SetupInputComponent() override;
	
	// Spawn a boid on click location
	UFUNCTION()
	void SpawnBoid();

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ABoid> BoidToSpawn;

	// Settings
	float MinSpeed = 0.3;
	float MaxSpeed = 1;
	float VisualRange = 10;
	float Speed = 30;
	int StartingBoids = 1000;
	FRotator Rot;

	FHitResult Hit;
	FVector MousePos;
	
	// Boids
	TArray<BoidPtr> Boids;
	TArray<ABoid*> CachedBoids;
	
	// Rules
	TArray<TUniquePtr<FBoidRules>> BoidRules;
	float* DefaultWeights;
	
	void InitializeRules();
	void ApplyBoidRules();
	void ApplyOneBoidRules(ABoid* Boid);
	void RandomizeBoidVelocity(ABoid* Boid);
	void LeftMouse();
	void RightMouse();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Weights")
		float SeparationWeight = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Weights")
		float CohesionWeight = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Weights")
		float AlignmentWeight = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Weights")
		float PointWeight = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Weights")
		float WallWeight = 3.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "AreaInfo")
		FVector2D WallArea = { 100, 100 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "AreaInfo")
		bool IsBounded = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "BoidDebug")
		bool DebugLines = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "BoidInput")
		bool EnableMouse = true;

	bool LeftClick;
	bool RightClick;

	//BoidSettings Settings;
	
public:
	virtual void Tick(float DeltaSeconds) override;
};