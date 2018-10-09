// Fill out your copyright notice in the Description page of Project Settings.

#include "CapturePointVolume.h"
#include "Engine.h"
#include "FPSCharacter.h"
#include "FPSGameStateBase.h"


ACapturePointVolume::ACapturePointVolume()
{
	//Register colission events Events
	OnActorBeginOverlap.AddDynamic(this, &ACapturePointVolume::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ACapturePointVolume::OnOverlapEnd);
}

void ACapturePointVolume::OnOverlapBegin(class AActor* overlappedActor, class AActor* otherActor)
{
	if (Role == ENetRole::ROLE_Authority)
	{
		AFPSCharacter* character = (AFPSCharacter*)otherActor;
		AFPSGameStateBase* gameState = (AFPSGameStateBase*)GetWorld()->GetGameState();

		if (character && gameState)
		{
			//adding a charcter to capture point array
			gameState->PlayerEnterCapturePoint(character->m_CharacterTeamIndex, m_CapturePointIndex);
		}
	}
}

void ACapturePointVolume::OnOverlapEnd(class AActor* overlappedActor, class AActor* otherActor)
{
	if (Role == ENetRole::ROLE_Authority)
	{
		AFPSCharacter* character = (AFPSCharacter*)otherActor;
		AFPSGameStateBase* gameState = (AFPSGameStateBase*)GetWorld()->GetGameState();
		if (character && gameState)
		{
			//removing a charcter from capture point array
			gameState->PlayerExitCapturePoint(character->m_CharacterTeamIndex, m_CapturePointIndex);
		}
	}
}