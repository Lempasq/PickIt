// Fill out your copyright notice in the Description page of Project Settings.


#include "../CollectibleItems/ItemSpawner.h"

#include "PickableItem.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AItemSpawner::AItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SpawnerStaticMesh(TEXT("/Game/Geometry/Meshes/1M_Cube_Chamfer"));
	
	SpawnerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMeshComponent"));
	if (SpawnerMeshComponent)
	{
		SpawnerMeshComponent->SetStaticMesh(SpawnerStaticMesh.Object);
		SpawnerMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	
	/* Set spawner material */
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ButtonMaterialObj(TEXT("/Game/Materials/M_SpawnerColor"));
	SpawnerMaterial = ButtonMaterialObj.Object;

	if (IsValid(SpawnerMeshComponent) && SpawnerMaterial)
	{
		SpawnerMeshComponent->SetMaterial(0, SpawnerMaterial);
	}

	/* Set pickable item BP */
	static ConstructorHelpers::FClassFinder<AActor> BPPickableItemClass(TEXT("/Game/PickableItems/BP_PickableItem"));
	BP_PickableItem = BPPickableItemClass.Class;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnerColorTimerDelegate.BindUFunction(this, FName("SetSpawnColor"), FLinearColor::Black, true, false);
	
	/* Button dynamic material */
	if (IsValid(SpawnerMeshComponent))
	{
		SpawnerDynamicMaterial = SpawnerMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
		DefaultSpawnerColor = SpawnerDynamicMaterial->K2_GetVectorParameterValue("SpawnerColor");
	}
}

bool AItemSpawner::IsSpawnerAvailable() const
{
	return !IsValid(PickableItem);
}

void AItemSpawner::SpawnItem_Implementation(FLinearColor NewSpawnerColor)
{
	FVector PickableItemSpawnLocation = this->GetActorLocation();
	PickableItem = Cast<APickableItem>(GetWorld()->SpawnActor(BP_PickableItem, &PickableItemSpawnLocation));
	PickableItem->SetParentItemSpawner(this);
	
	const bool bIsServer = GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer;
	if (bIsServer)
	{
		SetSpawnColor(NewSpawnerColor, false, true);
	}
}

void AItemSpawner::RemoveItem_Implementation()
{
	if (PickableItem)
	{
		PickableItem->DestroyItem();
		PickableItem = nullptr;
	}
}

void AItemSpawner::SetSpawnColor_Implementation(FLinearColor NewSpawnerColor, bool bResetColor, bool bShouldResetOnTimer)
{
	const bool bDynamicColorIsValid = IsValid(SpawnerDynamicMaterial);
	const bool bIsClient = GEngine->GetNetMode(GetWorld()) == NM_Client;
	if (bDynamicColorIsValid && bIsClient)
	{
		SpawnerDynamicMaterial->SetVectorParameterValue(FName("SpawnerColor"), bResetColor ? DefaultSpawnerColor : NewSpawnerColor);
	}
	
	const bool bIsServer = GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer;
	if (bShouldResetOnTimer && bIsServer)
	{
		GetWorldTimerManager().SetTimer(SpawnerColorTimerHandle, SpawnerColorTimerDelegate, 3.0f, false);
	}
}


