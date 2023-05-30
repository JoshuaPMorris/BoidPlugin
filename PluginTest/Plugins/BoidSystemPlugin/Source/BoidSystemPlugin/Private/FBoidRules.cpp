// Fill out your copyright notice in the Description page of Project Settings.


#include "FBoidRules.h"

#include <Engine/Engine.h>
#include <Engine/GameViewportClient.h>
#include <Kismet/GameplayStatics.h>

#include "Boid.h"
#include "DrawDebugHelpers.h"

FBoidRules::FBoidRules(const FBoidRules& SRules)
{
	Weight = SRules.Weight;
	IsEnabled = SRules.IsEnabled;
	Force2D = SRules.Force2D;
	DebugColor = SRules.DebugColor;
}

FVector2D FBoidRules::ComputeWeightedForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid)
{
	if (IsEnabled)
	{
		Force2D = GetBaseWeightMultiplier() * Weight * ComputeForce(Neighbourhood, Boid);
	}
	else
	{
		Force2D = FVector2D();
	}

	return Force2D;
}

FVector2D CohesionRule::ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid)
{
	FVector2D CohesionForce = FVector2D().ZeroVector;

	if (Neighbourhood.Num() > 0)
	{
		FVector2D Centre = FVector2D().ZeroVector;

		for (auto const& Neighbour : Neighbourhood)
		{
			Centre += FVector2D(Neighbour->GetActorLocation().X, Neighbour->GetActorLocation().Y);
		}
		
		Centre /= static_cast<float>(Neighbourhood.Num());

		FVector2D CentreDirection = Centre - Boid->GetPosition();

		CohesionForce = CentreDirection;
		CohesionForce.Normalize();
	}

	return CohesionForce;
}

FVector2D SeparationRule::ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid)
{
	FVector2D SeparationForce = FVector2D().ZeroVector;

	float DesiredDistance = DesiredMinimalDistance;
	
	if (Neighbourhood.Num() > 0)
	{
		FVector2D Position = Boid->GetPosition();
		int CountCloseNeighbours = 0;

		for (auto const& Neighbour : Neighbourhood)
		{
			float Distance = FVector2D::Distance(Position, Neighbour->GetPosition());

			if (Distance < DesiredDistance && Distance > 0)
			{
				FVector2D OpposedDirection = Position - Neighbour->GetPosition();
				OpposedDirection.Normalize();

				float ScalingFactor = FMath::Exp(-Distance);
				SeparationForce += OpposedDirection * ScalingFactor;
				CountCloseNeighbours++;
			}
		}

		if (CountCloseNeighbours > 0)
		{
			SeparationForce /= static_cast<float>(CountCloseNeighbours);
		}
	}

	SeparationForce.Normalize();
	
	return SeparationForce;
}

FVector2D AlignmentRule::ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid)
{
	FVector2D AverageVelocity = FVector2D().ZeroVector;

	if (Neighbourhood.Num() > 0)
	{
		for (auto const& Neighbour : Neighbourhood)
		{
			AverageVelocity += (Neighbour)->GetVelocity2D();
		}

		AverageVelocity /= static_cast<float>(Neighbourhood.Num());
	}

	AverageVelocity.Normalize();
	return AverageVelocity;
}

FVector2D BoundedAreaRule::ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid)
{
	FVector2D BoundedForce = FVector2D().ZeroVector;
	FVector2D Position = Boid->GetPosition();

	float Epsilon = 0.00001f;

	if (IsBounded) // If Bounded, use force to steer away from walls
	{
		// Min.
		if (Position.X < DesiredDistance)
		{
			BoundedForce.X += DesiredDistance / Position.X;
		}
		// Max.
		else if (Position.X > Width - DesiredDistance)
		{
			int Distance = Position.X - Width;
			BoundedForce.X += DesiredDistance / (Distance + Epsilon);
		}

		// Min.
		if (Position.Y < DesiredDistance)
		{
			BoundedForce.Y += DesiredDistance / Position.Y;
		}
		// Max.
		else if (Position.Y > Height - DesiredDistance)
		{
			int Distance = Position.Y - Height;
			BoundedForce.Y += DesiredDistance / (Distance + Epsilon);
		}

		return BoundedForce;
	}
	// Else teleport Boids to the other side of the bounded area.
	// Top Side.
	if (Position.Y > Height)
	{
		Boid->SetPosition(FVector2D(Position.X, Height - Height));
	}
	// Bottom Side.
	else if (Position.Y < Height - Height)
	{
		Boid->SetPosition(FVector2D(Position.X, Height));
	}

	// Left Side.
	else if (Position.X < Width - Width)
	{
		Boid->SetPosition(FVector2D(Width, Position.Y));
	}

	// Right Side.
	else if (Position.X > Width)
	{
		Boid->SetPosition(FVector2D(Width - Width, Position.Y));
	}

	return BoundedForce;
}

FVector2D PointRepulsionRule::ComputeForce(const TArray<ABoid*>& Neighbourhood, ABoid* Boid)
{
	FVector2D MouseForce;
	
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	
	if (World)
	{
		if (DebugLines) // Draw lines to neighbours. This just happen to be the most optimal spot to put this at the time.
		{
			for (int i = 0; i < Neighbourhood.Num(); i++)
			{
				if (FVector2D::Distance(Boid->GetPosition(), Neighbourhood[i]->GetPosition()) < 15)
				{
					DrawDebugLine(World, Boid->GetActorLocation(), Neighbourhood[i]->GetActorLocation(), FColor(255, 0, 0));
				}
			}
		}

		if (LeftClick || RightClick)
		{
			UGameViewportClient* ViewportClient = World->GetGameViewport();
			if (ViewportClient)
			{
				FViewport* Viewport = ViewportClient->Viewport;
				if (Viewport)
				{
					FIntPoint MousePosition;
					Viewport->GetMousePos(MousePosition);
				
					FVector WorldLocation;
					FVector WorldDirection;

					UGameplayStatics::DeprojectScreenToWorld(Controller, MousePosition, WorldLocation, WorldDirection);

					FHitResult HitING;
					if (World->LineTraceSingleByChannel(HitING, WorldLocation, WorldLocation + WorldDirection * 1000, ECC_Visibility))
					{
						MousePos = { HitING.Location.X, HitING.Location.Y };
					}
					else
					{
						MousePos = FVector2D();
					}
				}
			}

			FHitResult Hit;
			bool bHit = Controller->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
		
			if (bHit)
			{
				FVector2D Direction = MousePos - Boid->GetPosition();
				float Distance = Direction.Size();
			
				float DesiredDistance = 15.0f;


				if (Distance > DesiredDistance || LeftClick)
				{
					Direction.Normalize();
					MouseForce = Direction * 30;
				}
				else if (Distance < DesiredDistance)
				{
					Direction.Normalize();
					MouseForce = -Direction * 30;
				}
			
			
				if (LeftClick)
				{
					MouseForce = -MouseForce;
				}
			
				return MouseForce;
			}

			if (!bHit)
			{
				return FVector2D();
			}
		}
	}
	return FVector2D();
}