// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "CapturePointVolume.generated.h"

/**
 * Used to create capture point volumes that are used in a domination game mode.
 * every capture point knows there index in the array of capture points in FPSGameState
 */
UCLASS()
class NETFPS_API ACapturePointVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	ACapturePointVolume();

	UFUNCTION()
		void OnOverlapBegin(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void OnOverlapEnd(class AActor* overlappedActor, class AActor* otherActor);

	UPROPERTY()
		int32 m_CapturePointIndex;
};
