// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameMode.h"
#include "GunBase.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "FPSCharacter.h"
#include "FPSGameStateBase.h"
#include "FPSGameInstance.h"
#include "MyStructsAndEnums.h"
#include "CapturePointVolume.h"


AFPSGameMode::AFPSGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AFPSGameStateBase::StaticClass();
	DefaultPawnClass = nullptr;

	//getting Character_BP
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/BluePrints/BP_FPSCharacter.BP_FPSCharacter'"));
	if (PlayerPawnObject.Succeeded())
	{
		m_PlayerClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	DefaultPawnClass = nullptr;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	m_HasLoaded = false;
	m_ScoreGoal = 10;
}

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	//I must check if this operation has bean done, because PostLogin(APlayerController* NewPlayer) can be called before BeginPlay()
	if (!m_HasLoaded)
	{
		m_GameStateBase = Cast<AFPSGameStateBase>(GameState);

		m_GameStateBase->m_ScoreGoal = m_ScoreGoal;

		//getting all spawn points in level
		for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			APlayerStart *spawn = *ActorItr;
			FString spawnName = ActorItr->GetName();

			//adding spwan point to appropriate team spawn point array
			if (spawnName.Contains(TEXT("Zero")))
				m_TeamZeroSpawn.Add(spawn);
			else if (spawnName.Contains(TEXT("One")))
				m_TeamOneSpawn.Add(spawn);
		}
		m_HasLoaded = true;
	}
}

void AFPSGameMode::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("FPS Game Mode"));
	}
}

void AFPSGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFPSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//check if beginPlay lauched, postlogin can run before it.
	if (!m_HasLoaded)
	{
		m_GameStateBase = Cast<AFPSGameStateBase>(GameState);

		m_GameStateBase->m_ScoreGoal = m_ScoreGoal;

		for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators. 
			APlayerStart *spawn = *ActorItr;
			FString spawnName = ActorItr->GetName();

			if (spawnName.Contains(TEXT("Zero")))
				m_TeamZeroSpawn.Add(spawn);
			else if (spawnName.Contains(TEXT("One")))
				m_TeamOneSpawn.Add(spawn);
		}
		m_HasLoaded = true;
	}

	//choose team for player
	int32 smallestTeamIndex = 0;
	for (int t = 1; t < m_GameStateBase->m_MaxTeams; t++)
	{
		if (m_GameStateBase->m_Teams[t].m_TeamSize < m_GameStateBase->m_Teams[smallestTeamIndex].m_TeamSize)
		{
			smallestTeamIndex = t;
		}
	}

	//find the player a slot in the team
	int32 playerSlot = -1;
	for (int p = 0; p < m_GameStateBase->m_TeamMaxSize; p++)
	{
		//found a spot for the player
		if (m_GameStateBase->m_Teams[smallestTeamIndex].m_Players[p] == nullptr)
		{
			playerSlot = p;
			m_GameStateBase->m_Teams[smallestTeamIndex].m_TeamSize++;
			m_GameStateBase->m_Teams[smallestTeamIndex].m_Players[p] = NewPlayer;

			//assign a team
			FVector location;
			FRotator rotation;
			if (m_TeamOneSpawn[playerSlot] && m_TeamZeroSpawn[playerSlot])
			{
				switch (smallestTeamIndex)
				{
				case 0:
					location = m_TeamZeroSpawn[playerSlot]->GetActorLocation();
					rotation = m_TeamZeroSpawn[playerSlot]->GetActorRotation();
					break;

				case 1:
					location = m_TeamOneSpawn[playerSlot]->GetActorLocation();
					rotation = m_TeamOneSpawn[playerSlot]->GetActorRotation();
					break;

				default:
					break;
				}
			}

			//spawn character
			FActorSpawnParameters params;
			params.Owner = NewPlayer;
			AFPSCharacter *tempCharacter = (AFPSCharacter*)GetWorld()->SpawnActor<AFPSCharacter>(m_PlayerClass, location, rotation, params);
			NewPlayer->Possess(tempCharacter);
			tempCharacter->InIt(smallestTeamIndex, playerSlot);
			//need to force client rotation
			tempCharacter->GetController()->ClientSetRotation(rotation, false);
			break;
		}
	}

	//if there is no space left in teams
	if (playerSlot == -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NO SLOTS IN TEAMS LEFT"));
		}
	}

}

void AFPSGameMode::RespawnCharacter(AFPSCharacter* Character, int32 CharacterTeamIndex, int32 CharacterIndex)
{
	// basic respawn system, I have plans on making it more complex and dynamic, so it can work with any number of teams and players.
	//at the moment it works for only 2 teams
	switch (CharacterTeamIndex)
	{
	case 0:
		Character->GetController()->ClientSetRotation(m_TeamZeroSpawn[CharacterIndex]->GetActorRotation());
		Character->SetActorLocation(m_TeamZeroSpawn[CharacterIndex]->GetActorLocation());
		break;

	case 1:
		Character->GetController()->ClientSetRotation(m_TeamOneSpawn[CharacterIndex]->GetActorRotation());
		Character->SetActorLocation(m_TeamOneSpawn[CharacterIndex]->GetActorLocation());
		break;

	default:
		break;
	}
}