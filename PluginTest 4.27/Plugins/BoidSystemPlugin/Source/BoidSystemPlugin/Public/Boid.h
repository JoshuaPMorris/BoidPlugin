// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBoidRules.h"

#include "Boid.generated.h"

UCLASS()
class BOIDSYSTEMPLUGIN_API ABoid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABoid();
	virtual void BeginDestroy() override;

	void ResetAcceleration();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void ComputeNeighbourhood();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Getters
	float GetVisualRange() const			{ return VisualRange; }
	FVector2D GetVelocity2D() const			{ return Velocity; }
	FVector2D GetPosition() const			{ return FVector2D(this->GetActorLocation().X, this->GetActorLocation().Y); }
	FRotator GetRotation() const			{ return this->GetActorRotation(); }
	float GetSpeed() const					{ return Speed; }
	FVector2D GetAcceleration() const		{ return Acceleration; }
	float GetMaxAcceleration() const		{ return MaxAcceleration; }
	bool GetIfConstantSpeed() const			{ return HasConstantSpeed; }
	
	// Setters
	void SetVisualRange(float _VisualRange) 			{ VisualRange = _VisualRange; }
	void SetVelocity2D(FVector2D _Velocity)				{ Velocity = _Velocity; }
	void SetPosition(FVector2D _Position)				{ this->SetActorLocation(FVector(_Position.X, _Position.Y, 1)); }
	void SetRotation(FRotator _Rotator)					{ this->SetActorRotation(_Rotator); }
	void SetSpeed(float _Speed)							{ Speed = _Speed; }
	void SetAcceleration(FVector2D _Acceleration)		{ Acceleration = _Acceleration; }
	void SetMaxAcceleration(float _MaxAcceleration)		{ MaxAcceleration = _MaxAcceleration; }
	void SetIfConstantSpeed(bool _HasConstantSpeed)		{ HasConstantSpeed = _HasConstantSpeed; }
	void SetBoidRules(TArray<TUniquePtr<FBoidRules>> const& NewRules)
	{
		Rules.Empty();
	
		for (auto& Rule : NewRules)
		{
			Rules.Add(Rule->Clone());
		}
	}
	void ApplyForce(FVector2D Force);
	void BoidRotation();

	FVector2D Position;
	float VisualRange;
	FVector2D Velocity;
	float Speed;
	bool HasConstantSpeed;
	
	FVector2D Acceleration;
	float MaxAcceleration;

	
	UPROPERTY(VisibleAnywhere)
	TArray<ABoid*> ListOfBoidsInVision;

	TArray<TUniquePtr<FBoidRules>> Rules;
};