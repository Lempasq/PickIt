// Fill out your copyright notice in the Description page of Project Settings.


#include "../CollectibleItems/ItemSpawnButton.h"

#include "ItemSpawner.h"
#include "PickableItem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "PickIt/PickItGameMode.h"

// Sets default values
AItemSpawnButton::AItemSpawnButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ButtonStaticMesh(TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube"));
	ButtonStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonStaticMeshComponent"));
	if (ButtonStaticMeshComponent)
	{
		ButtonStaticMeshComponent->SetStaticMesh(ButtonStaticMesh.Object);
		ButtonStaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	
	ButtonBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ButtonBoxCollision"));
	if (ButtonBoxCollision)
	{
		ButtonBoxCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	
	/* Set button material */
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ButtonMaterialObj(TEXT("/Game/Materials/M_ItemSpawnButton"));
	ButtonMaterial = ButtonMaterialObj.Object;

	if (IsValid(ButtonStaticMeshComponent) && ButtonMaterial)
	{
		ButtonStaticMeshComponent->SetMaterial(0, ButtonMaterial);
	}

	bReplicates = true;
}

// Called when the game starts or when spawned
void AItemSpawnButton::BeginPlay()
{
	Super::BeginPlay();
	
	/* Button dynamic material */
	if (IsValid(ButtonStaticMeshComponent))
	{
		ButtonDynamicMaterial = ButtonStaticMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
		DefaultButtonColor = ButtonDynamicMaterial->K2_GetVectorParameterValue("AvailableButtonColor");
	}
	
	ButtonTimerDelegate.BindUFunction(this, FName("SwitchButtonState"), true);
}

// Called every frame
void AItemSpawnButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer)
	{
		TArray<APickItCharacter*> OverlappedPlayers = GetOverlappedPlayers();

		/* On button available, and it was activated by someone,then reset it till next frame */
		if (IsButtonAvailable() && IsValid(TriggeredPlayer))
		{
			TriggeredPlayer = nullptr;
			RemovePlayersFromButton();
			OverlappedPlayers.SetNum(0);
			SwitchButtonState(true);
		}
		
		/* Check if player was on the button */
		if (TriggeredPlayer)
		{
			if (OverlappedPlayers.Num() > 0)
			{
				// Revive each player that != triggered player
				if (OverlappedPlayers.Num() > 1)
				{
					RemovePlayersFromButton();
				}
				
				if (IsTimerActive())
				{
					PauseTimerCooldown();
				}
			}
			/* If player was on the button, but now its gone and there is no player left on it, then start timer to reset button */
			else
			{
				if (!IsTimerActive())
				{
					StartTimerCooldown();
				}
			}
		}
		else
		{
			if (OverlappedPlayers.Num() > 0)
			{
				if (!IsTimerActive())
				{
					if (IsButtonAvailable())
					{
						TriggeredPlayer = OverlappedPlayers[0];
						ActivateButton();
					}
					else
					{
						RemovePlayersFromButton();
					}
				}
				else
				{
					RemovePlayersFromButton();
					ResetTimerCooldown();
				}
			}
		}
	}
}

void AItemSpawnButton::StartTimerCooldown()
{
	GetWorldTimerManager().SetTimer(ButtonTimerHandle, ButtonTimerDelegate, 2.0f, false);
}

void AItemSpawnButton::PauseTimerCooldown()
{
	if (!GetWorldTimerManager().IsTimerPaused(ButtonTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(ButtonTimerHandle);
	}
}

void AItemSpawnButton::UnPauseTimerCooldown()
{
	if (GetWorldTimerManager().IsTimerPaused(ButtonTimerHandle))
	{
		GetWorldTimerManager().UnPauseTimer(ButtonTimerHandle);
	}
}

void AItemSpawnButton::ResetTimerCooldown()
{
	GetWorldTimerManager().ClearTimer(ButtonTimerHandle);
	StartTimerCooldown();
}

void AItemSpawnButton::RemovePlayersFromButton()
{
	TArray<APickItCharacter*> OverlappedPlayers = GetOverlappedPlayers();

	for (APickItCharacter* OverlappedPlayer : OverlappedPlayers)
	{
		FBox ReviveLocationBox;
		APickItGameMode* GameMode = GetWorld()->GetAuthGameMode<APickItGameMode>();
		if (IsValid(GameMode))
		{
			ReviveLocationBox = GameMode->GetPlayerLocationBox();
		}
		else
		{
			ReviveLocationBox = {};
		}

		if (OverlappedPlayer != TriggeredPlayer)
		{
			OverlappedPlayer->ReviveCharacter(ReviveLocationBox);
		}
	}
}

void AItemSpawnButton::SwitchButtonState(bool bIsAvailable)
{
	if (bIsAvailable)
	{
		GetWorldTimerManager().ClearTimer(ButtonTimerHandle);
	}
	else
	{
		ResetTimerCooldown();
	}
	
	SwitchButtonColor(bIsAvailable);
}

void AItemSpawnButton::SwitchButtonColor_Implementation(bool bIsAvailable)
{
	if (GEngine->GetNetMode(GetWorld()) == NM_Client)
	{
		if (bIsAvailable)
		{
			if (IsValid(ButtonDynamicMaterial))
			{
				ButtonDynamicMaterial->SetScalarParameterValue(FName("ButtonDarkness"), 0.0f);
			}
		}
		else
		{
			if (IsValid(ButtonDynamicMaterial))
			{
				ButtonDynamicMaterial->SetScalarParameterValue(FName("ButtonDarkness"), 0.8f);
			}
		}
	}
}

TArray<APickItCharacter*> AItemSpawnButton::GetOverlappedPlayers()
{
	TArray<AActor*> OverlappingButtonActors;
	ButtonBoxCollision->GetOverlappingActors(OverlappingButtonActors, APickItCharacter::StaticClass());
	
	TArray<APickItCharacter*> OverlappingButtonPlayers;
	for (AActor* OverlappedActor : OverlappingButtonActors)
	{
		OverlappingButtonPlayers.Add(Cast<APickItCharacter>(OverlappedActor));
	}
	return OverlappingButtonPlayers;
}

TArray<AItemSpawner*> AItemSpawnButton::GetItemSpawners()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemSpawner::StaticClass(), FoundActors);

	TArray<AItemSpawner*> FoundSpawners;
	for (AActor* Actor : FoundActors)
	{
		FoundSpawners.Add(Cast<AItemSpawner>(Actor));
	}
	return FoundSpawners;
}

bool AItemSpawnButton::IsButtonAvailable()
{
	TArray<AItemSpawner*> ItemSpawners = GetItemSpawners();

	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickableItem::StaticClass(), FoundItems);
	
	if (FoundItems.Num() > 0)
	{
		RemovePlayersFromButton();
		return false;
	}
	
	for (AItemSpawner* ItemSpawner : ItemSpawners)
	{
		if (ItemSpawner->IsSpawnerAvailable())
		{
			return true;
		}
	}

	RemovePlayersFromButton();
	return false;
}

bool AItemSpawnButton::IsTimerActive()
{
	return GetWorldTimerManager().IsTimerActive(ButtonTimerHandle);
}

void AItemSpawnButton::ActivateButton()
{
	TArray<AItemSpawner*> ItemSpawners = GetItemSpawners();
	const int32 TargetSpawnerIndex = UKismetMathLibrary::RandomIntegerInRange(0, ItemSpawners.Num() - 1);
	
	ItemSpawners[TargetSpawnerIndex]->SpawnItem(DefaultButtonColor);
	SwitchButtonState(false);
}

