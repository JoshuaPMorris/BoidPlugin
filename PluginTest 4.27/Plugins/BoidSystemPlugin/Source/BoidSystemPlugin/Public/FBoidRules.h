// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/PlayerController.h>

#include "CoreMinimal.h"

class ABoid;

/**
 * 
 */
class BOIDSYSTEMPLUGIN_API FBoidRules
{
public:
	FBoidRules(const FBoidRules& SRules); // All rules for boids
	
	FVector2D ComputeWeightedForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid);

	virtual TUniquePtr<FBoidRules> Clone() = 0;

protected:
	FColor DebugColor;

public:
	float Weight;
	bool IsEnabled;
	
protected:
	FVector2D Force2D;
	
	FBoidRules(FColor DebugColor, float _Weight, bool _IsEnabled) :
		DebugColor(DebugColor),
		Weight(_Weight),
		IsEnabled(_IsEnabled),
		Force2D(FVector())
	{}
	virtual FVector2D ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid) = 0;
	virtual float GetBaseWeightMultiplier() { return 1; }
};

class BOIDSYSTEMPLUGIN_API CohesionRule : public FBoidRules
{
public:
	CohesionRule(float Weight = 1, bool IsEnabled = true) : FBoidRules(FColor::Cyan, Weight, IsEnabled) {} // Median position of neighbours.

	FVector2D ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid) override;
	virtual float GetBaseWeightMultiplier() override { return 1; }

	TUniquePtr<FBoidRules> Clone() override
	{
		return MakeUnique<CohesionRule>(*this);
	}
};

class BOIDSYSTEMPLUGIN_API SeparationRule : public FBoidRules
{
public:
	SeparationRule(float DesiredSeparation = 20, float Weight = 1, bool IsEnabled = true) : FBoidRules(FColor::Red, Weight, IsEnabled) {} // Keep distance from boids.

	FVector2D ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid) override;
	virtual float GetBaseWeightMultiplier() override { return 1; }

	TUniquePtr<FBoidRules> Clone() override
	{
		return MakeUnique<SeparationRule>(*this);
	}

private:	
	float DesiredMinimalDistance = 10;
};

class BOIDSYSTEMPLUGIN_API AlignmentRule : public FBoidRules
{
public:
	AlignmentRule(float Weight = 1, bool IsEnabled = true) : FBoidRules(FColor::Yellow, Weight, IsEnabled) {} // Median direction of neighbours.

	FVector2D ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid) override;
	virtual float GetBaseWeightMultiplier() override { return 1; }

	TUniquePtr<FBoidRules> Clone() override
	{
		return MakeUnique<AlignmentRule>(*this);
	}
};

class BOIDSYSTEMPLUGIN_API BoundedAreaRule : public FBoidRules
{
public:
	BoundedAreaRule(int _Height, int _Width, int _DistanceFromBorder, float Weight = 1, bool _IsBounded = true, bool IsEnabled = true) :
		FBoidRules(FColor::Purple, Weight, IsEnabled), Height(_Height), DesiredDistance(_DistanceFromBorder), Width(_Width),
		IsBounded(_IsBounded) {}

	BoundedAreaRule(const BoundedAreaRule& SRules) : FBoidRules(SRules)
	{
		Height = SRules.Height;
		Width = SRules.Width;
		DesiredDistance = SRules.DesiredDistance;
		IsBounded = SRules.IsBounded;
	}

	FVector2D ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid) override;
	virtual float GetBaseWeightMultiplier() override { return 1; }

	TUniquePtr<FBoidRules> Clone() override
	{
		return MakeUnique<BoundedAreaRule>(*this);
	}

private:
	int Height;
	int DesiredDistance;
	int Width;
	bool IsBounded;
};

class BOIDSYSTEMPLUGIN_API PointRepulsionRule : public FBoidRules
{
public:
	PointRepulsionRule(float weight = 1.0f, bool IsEnabled = true, bool IsRepulsive_ = false,
	UObject* _WorldContextObject = nullptr, APlayerController* _Controller = nullptr, bool _DebugLines = true,
		bool _EnablePress = true, bool _LeftClick = false, bool _RightClick = false) :
		FBoidRules(FColor::Magenta, weight, IsEnabled), IsRepulsive(IsRepulsive_),
		WorldContextObject(_WorldContextObject), Controller(_Controller),DebugLines(_DebugLines), EnablePress(_EnablePress), LeftClick(_LeftClick), RightClick(_RightClick) {}

	PointRepulsionRule(const PointRepulsionRule& SRules) : FBoidRules(SRules)
	{
		IsRepulsive = SRules.IsRepulsive;
		WorldContextObject = SRules.WorldContextObject;
		Controller = SRules.Controller;
		DebugLines = SRules.DebugLines;
		EnablePress = SRules.EnablePress;
		LeftClick = SRules.LeftClick;
		RightClick = SRules.RightClick;
	}
	FVector2D ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid) override;
	virtual float GetBaseWeightMultiplier() override { return 0.1f; }

	TUniquePtr<FBoidRules> Clone() override
	{
		return MakeUnique<PointRepulsionRule>(*this);
	}	

private:
	bool IsRepulsive; // Will attract instead if false
	UObject* WorldContextObject;
	APlayerController* Controller;
	FVector2D MousePos;
	bool DebugLines;
	bool EnablePress;
	bool LeftClick;
	bool RightClick;
};