// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "PickItSpectator.generated.h"

/**
 * 
 */
UCLASS()
class PICKIT_API APickItSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	FVector SpectatorLocation = FVector(4500.f,0.0f,3000.f);
	
	UPROPERTY(EditAnywhere)
	FRotator SpectatorRotation = FRotator(-49.f, 180.f, .0f);
	
public:
	APickItSpectator();

	virtual void BeginPlay() override;;
	void IsSpectatorCanMove(const bool bCanMove);
	
};
