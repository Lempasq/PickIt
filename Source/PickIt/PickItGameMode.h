// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PickItCharacter.h"
#include "GameFramework/GameMode.h"
#include "PickItGameMode.generated.h"

class APickableItem;

/**
 * 
 */
UCLASS()
class PICKIT_API APickItGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TSubclassOf<APawn> MyPlayerCharacter = nullptr;
	
	UPROPERTY()
	TSubclassOf<APawn> MySpectatorEye = nullptr;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 ScoreToWin = 30;

	UPROPERTY()
	TArray<APickItCharacter*> PlayersPawns;

	/** Player spawn area */
	UPROPERTY()
	FBox PlayerSpawnLocationBox;

	UPROPERTY()
	FVector PlayerSpawnLocationPoint = FVector(0.0f, 0.0f, 230.0f);

	UPROPERTY()
	float PlayerSpawnLocationRadius = 1200.0f;
	/*********************/

	/** Timer manager */
	FTimerHandle StartGameDelayHandle;
	FTimerDelegate StartGameDelayDelegate;
	
public:
	APickItGameMode();

	virtual void Tick(float DeltaSeconds) override;

	virtual void StartPlay() override;

	/** On new player login */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** On player controller get own pawn */
	UFUNCTION()
	void PossessPlayers();

	/** Record new pawn in PlayersPawns array */
	UFUNCTION()
	void AddPlayerPawn(APickItCharacter* PlayerPawn);

	/** Getter */
	UFUNCTION()
	FBox GetPlayerLocationBox();

	/** Getter */
	UFUNCTION()
	int32 GetScoreToWin() const;
	
};
