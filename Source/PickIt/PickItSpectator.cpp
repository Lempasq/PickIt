// Fill out your copyright notice in the Description page of Project Settings.


#include "PickItSpectator.h"

#include "GameFramework/PawnMovementComponent.h"

APickItSpectator::APickItSpectator()
{
	this->SetActorLocation(SpectatorLocation);
	this->SetActorRotation(SpectatorRotation);
}

void APickItSpectator::BeginPlay()
{
	Super::BeginPlay();
	
	bAddDefaultMovementBindings = false;
}

void APickItSpectator::IsSpectatorCanMove(const bool bCanMove)
{
	bAddDefaultMovementBindings = bCanMove;
}
