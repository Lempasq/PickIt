// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PickItCharacter.h"
#include "CollectibleItems/PickableItem.h"
#include "GameFramework/GameState.h"
#include "PickItGameState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PICKIT_API APickItGameState : public AGameState
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TSubclassOf<APawn> MyPlayerCharacter = nullptr;
	
public:
	APickItGameState();

	/** Spawn new player, and record it in game mode */
	UFUNCTION(Server, Unreliable)
	void SpawnNewPlayer();
	
};
