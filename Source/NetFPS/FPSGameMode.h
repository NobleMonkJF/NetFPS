// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"


/**
 * main gamemode contains game rules and restriction of the game, also takes care of handeling new players joinig session and spawning characters.
 */
UCLASS()
class NETFPS_API AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSGameMode(const class FObjectInitializer& ObjectInitializer);

	UFUNCTION()
		virtual void StartPlay() override;

	UFUNCTION()
		virtual void BeginPlay() override;

	UFUNCTION()
		virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void PostLogin(class APlayerController* NewPlayer) override;

	UFUNCTION()
		void RespawnCharacter(class AFPSCharacter* Character, int32 CharacterTeamIndex, int32 CharacterIndex);

private:
	UPROPERTY()
		class AFPSGameStateBase* m_GameStateBase;

	UPROPERTY()
		TSubclassOf<class AFPSCharacter> m_PlayerClass;

	UPROPERTY(EditAnywhere)
		TArray <class APlayerStart*> m_TeamZeroSpawn;

	UPROPERTY(EditAnywhere)
		TArray <class APlayerStart*> m_TeamOneSpawn;

	UPROPERTY()
		bool m_HasLoaded;

	UPROPERTY()
		float m_ScoreGoal;

};


