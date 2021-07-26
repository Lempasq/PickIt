// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

class APickableItem;

UCLASS()
class PICKIT_API AItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TSubclassOf<AActor> BP_PickableItem = nullptr;

	UPROPERTY()
	FLinearColor DefaultSpawnerColor = FLinearColor::Black;

	UPROPERTY()
	UMaterialInterface* SpawnerMaterial = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* SpawnerDynamicMaterial = nullptr;

	/* Spawner color timer */
	FTimerHandle SpawnerColorTimerHandle;
	FTimerDelegate SpawnerColorTimerDelegate;

	/* Ptr to item, that was created by this spawner */
	UPROPERTY()
	APickableItem* PickableItem = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* SpawnerMeshComponent;

public:	

	/* Check, if this spawner doesn't have exising spawned item */
	bool IsSpawnerAvailable() const;
	
	/* Spawn new item on location, relative to this actor location, and set new color, relative to triggered button for 3s */
	UFUNCTION(NetMulticast, Unreliable)
	void SpawnItem(FLinearColor NewSpawnerColor);

	/** Remove created item */
	UFUNCTION(NetMulticast, Unreliable)
	void RemoveItem();

	/** Change spawner primitive mesh color
	 * @param NewSpawnerColor new color for primitive mesh
	 * @param bResetColor reset color for primitive mesh to default value
	 * @param bShouldResetOnTimer reset color for primitive mesh to default value after 3s
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void SetSpawnColor(FLinearColor NewSpawnerColor, bool bResetColor, bool bShouldResetOnTimer);
	
};
