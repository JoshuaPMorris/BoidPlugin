// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PlayerCamera.generated.h"

UCLASS()
class BOIDSYSTEMPLUGIN_API APlayerCamera final : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerCamera();
	virtual ~APlayerCamera() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input variables
	FVector2D MovementInput;
	
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;
	
	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComp;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComp;
};
