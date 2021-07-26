// Fill out your copyright notice in the Description page of Project Settings.


#include "../CollectibleItems/PickableItem.h"

#include "ItemSpawner.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PickIt/PickItCharacter.h"

// Sets default values
APickableItem::APickableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemStaticMesh(TEXT("/Game/Geometry/Meshes/Sphere"));

	ItemStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	ItemStaticMeshComponent->SetStaticMesh(ItemStaticMesh.Object);
	ItemStaticMeshComponent->SetCollisionProfileName("OverlapAll");
	ItemStaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	this->SetReplicates(true);
}

// Called when the game starts or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer)
	{	
		CheckOverlappedActors();
	}
}

void APickableItem::CheckOverlappedActors()
{
	TArray<AActor*> OverlappingItemActors;
	GetOverlappingActors(OverlappingItemActors, APickItCharacter::StaticClass());
		
	for (AActor* OverlappedActor : OverlappingItemActors)
	{
		APickItCharacter* OverlappedPlayer = Cast<APickItCharacter>(OverlappedActor);
		if (IsValid(OverlappedPlayer))
		{
			OverlappedPlayer->IncreasePlayerScore();
		}
		
		if (ItemSpawner)
		{
			ItemSpawner->RemoveItem();
		}
		else
		{
			DestroyItem();
		}
		
		break;
	}
}

void APickableItem::SetColor(UMaterialInterface* MaterialInterface)
{
}

void APickableItem::SetColor(UMaterialInstance* MaterialInterface)
{
}

void APickableItem::SetParentItemSpawner(AItemSpawner* _ItemSpawner)
{
	ItemSpawner = _ItemSpawner;
}

void APickableItem::DestroyItem_Implementation()
{
	this->Destroy();
}

