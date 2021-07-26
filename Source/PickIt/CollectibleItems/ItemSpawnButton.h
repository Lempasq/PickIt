// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "PickIt/PickItCharacter.h"

#include "ItemSpawnButton.generated.h"

class AItemSpawner;

UCLASS()
class PICKIT_API AItemSpawnButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawnButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	FLinearColor DefaultButtonColor = FLinearColor::Black;

	UPROPERTY()
	UMaterialInterface* ButtonMaterial = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* ButtonDynamicMaterial = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* ButtonStaticMeshComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* ButtonBoxCollision = nullptr;

	/** Ptr to the player, that triggered this button */
	UPROPERTY()
	APickItCharacter* TriggeredPlayer = nullptr;

	/** Button timer */
	FTimerHandle ButtonTimerHandle;
	FTimerDelegate ButtonTimerDelegate;

	/** Button timer manager */
	void StartTimerCooldown();
	void PauseTimerCooldown();
	void UnPauseTimerCooldown();
	void ResetTimerCooldown();
	bool IsTimerActive();

	/** Return all players, that stand on this button */
	TArray<APickItCharacter*> GetOverlappedPlayers();
	TArray<AItemSpawner*>  GetItemSpawners();

	/** Button state */
	bool IsButtonAvailable();

	void ActivateButton();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Switch state between available / unavailable
	 * Also switching its color, relative to button state
	 */
	UFUNCTION(BlueprintCallable)
	void SwitchButtonState(bool bIsAvailable);

	UFUNCTION(NetMulticast, Unreliable)
	void SwitchButtonColor(bool bIsAvailable);

	/** If player stand on this button, and its unavailable, and this player is not the TriggeredPlayer, then Revive him */
	UFUNCTION()
	void RemovePlayersFromButton();

};
