// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AFPSCharacter;

#include "CoreMinimal.h"
#include "MyStructsAndEnums.generated.h"

/**
 * this is the team struct contains: player in the team, number of players inside capture points, team size, team score, team colour
 */
USTRUCT()
struct FMyTeamStruct
{
	GENERATED_BODY()

	UPROPERTY()
		TArray<APlayerController*> m_Players;

	UPROPERTY()
		TArray <int32> m_NumberOfPlayersInsideCapturePoint;

	UPROPERTY()
		int32 m_TeamSize = 0;

	UPROPERTY()
		float m_TeamScore = 0;

	UPROPERTY()
		FLinearColor m_TeamColour;
};

// this is the team score struct, it used to sort all the socres
// contains : scores and the team index the score belongs to.
USTRUCT()
struct FTeamScores
{
	GENERATED_BODY()

	UPROPERTY()
		float m_Score = 0;

	UPROPERTY()
		int32 m_TeamIndex = 0;
};

// this is the team color struct, it used to sort all the team colors
USTRUCT()
struct FTeamColors
{
	GENERATED_BODY()
	
	//blue
	static const FLinearColor LC_TEAM_ZERO;

	//orange
	static const FLinearColor LC_TEAM_ONE;
};


