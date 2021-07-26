// Fill out your copyright notice in the Description page of Project Settings.


#include "PickItGameState.h"

#include "PickItCharacter.h"
#include "PickItGameMode.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

APickItGameState::APickItGameState()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));

	if (PlayerPawnBPClass.Class)
	{
		MyPlayerCharacter = PlayerPawnBPClass.Class;
	}
}

void APickItGameState::SpawnNewPlayer_Implementation()
{
	const FVector SpawnLocation = FVector(0.0f, 0.0f, 230.0f);
	APickItCharacter* PickItCharacter = Cast<APickItCharacter>(GetWorld()->SpawnActor(MyPlayerCharacter, &SpawnLocation));

	APickItGameMode* GameMode = GetWorld()->GetAuthGameMode<APickItGameMode>();
	if (IsValid(GameMode))
	{
		GameMode->AddPlayerPawn(PickItCharacter);
	}
}
