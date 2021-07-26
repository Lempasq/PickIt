// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "PickableItem.generated.h"

class AItemSpawner;

UCLASS()
class PICKIT_API APickableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* ItemStaticMeshComponent = nullptr;	

	/* Ptr to spawner, that created this item */
	UPROPERTY()
	AItemSpawner* ItemSpawner = nullptr;	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Looking for all overlapped players, and if there is one at least, then add him one score and destroy this item */
	void CheckOverlappedActors();
	
	UFUNCTION(Server, Reliable)
	void DestroyItem();

	/* Set color, that meant to be relative to parent spawner color */
	void SetColor(UMaterialInterface* MaterialInterface);
	void SetColor(UMaterialInstance* MaterialInterface);

	void SetParentItemSpawner(AItemSpawner* _ItemSpawner);
	
};
