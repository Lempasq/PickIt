// Fill out your copyright notice in the Description page of Project Settings.


#include "PickItGameMode.h"
#include "PickItCharacter.h"
#include "PickItGameState.h"
#include "PickItPlayerController.h"
#include "PickItSpectator.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

APickItGameMode::APickItGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));

	if (PlayerPawnBPClass.Class)
	{
		MyPlayerCharacter = PlayerPawnBPClass.Class;
	}

	/* Set player pawn */
	// if (MyPlayerCharacter)
	// {
	// 	DefaultPawnClass = MyPlayerCharacter;
	// }

	DefaultPawnClass = APickItSpectator::StaticClass();
	
	PlayerControllerClass = APickItPlayerController::StaticClass();
	GameStateClass = APickItGameState::StaticClass();

	FVector PlayerSpawnLocationBoxMin;
	PlayerSpawnLocationBoxMin.X = PlayerSpawnLocationPoint.X - PlayerSpawnLocationRadius;
	PlayerSpawnLocationBoxMin.Y = PlayerSpawnLocationPoint.Y - PlayerSpawnLocationRadius;
	PlayerSpawnLocationBoxMin.Z = PlayerSpawnLocationPoint.Z;

	FVector PlayerSpawnLocationBoxMax;
	PlayerSpawnLocationBoxMax.X = PlayerSpawnLocationPoint.X + PlayerSpawnLocationRadius;
	PlayerSpawnLocationBoxMax.Y = PlayerSpawnLocationPoint.Y + PlayerSpawnLocationRadius;
	PlayerSpawnLocationBoxMax.Z = PlayerSpawnLocationPoint.Z;
	
	PlayerSpawnLocationBox = { PlayerSpawnLocationBoxMin, PlayerSpawnLocationBoxMax };
}

void APickItGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

void APickItGameMode::StartPlay()
{
	StartGameDelayDelegate.BindUFunction(this, FName("PossessPlayers"));
	GetWorldTimerManager().SetTimer(StartGameDelayHandle, StartGameDelayDelegate, 5.0f, false);

	Super::StartPlay();
}

void APickItGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APickItGameState* PickItGameState = GetGameState<APickItGameState>();
	if (PickItGameState)
	{
		PickItGameState->SpawnNewPlayer();
	}
}

int32 APickItGameMode::GetScoreToWin() const
{
	return ScoreToWin;
}

void APickItGameMode::PossessPlayers()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickItPlayerController::StaticClass(), FoundActors);

	for (int32 i = 0; i < FoundActors.Num(); i++)
	{
		if (IsValid(PlayersPawns[i]) && IsValid(FoundActors[i]))
		{
			APickItPlayerController* PlayerController = Cast<APickItPlayerController>(FoundActors[i]);
			if (PlayerController)
			{
				PlayerController->Possess(PlayersPawns[i]);
			}

			APickItCharacter* PlayerCharacter = Cast<APickItCharacter>(PlayersPawns[i]);

			if (IsValid(PlayerCharacter))
			{
				PlayerCharacter->ReviveCharacter(PlayerSpawnLocationBox);
			}
		}
	}
}

void APickItGameMode::AddPlayerPawn(APickItCharacter* PlayerPawn)
{
	if (IsValid(PlayerPawn))
	{
		PlayersPawns.Add(PlayerPawn);
	}
}

FBox APickItGameMode::GetPlayerLocationBox()
{
	return  PlayerSpawnLocationBox;
}
