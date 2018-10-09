// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyStructsAndEnums.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameStateBase.generated.h"

/**
 * main GameState class, contais teams, max team size and max amount of team. It handles giveing points out, score sorting and setting HUD elements.
 */
UCLASS()
class NETFPS_API AFPSGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	 
public:
	AFPSGameStateBase(const class FObjectInitializer& ObjectInitializer);
	
	UFUNCTION()
		virtual void BeginPlay() override;

	UFUNCTION()
		virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void ChangeWidget(TSubclassOf<class UUserWidget> NewWidgetClass);

	UFUNCTION()
		void PlayerEnterCapturePoint(int32 TeamIndex, int32 CapturePointIndex);

	UFUNCTION()
		void PlayerExitCapturePoint(int32 TeamIndex, int32 CapturePointIndex);

	UPROPERTY(EditAnywhere, blueprintReadWrite)
		int32 m_TeamMaxSize;

	UPROPERTY(EditAnywhere, blueprintReadWrite)
		int32 m_MaxTeams;

	UPROPERTY(Replicated)
		TArray<struct FMyTeamStruct> m_Teams;

	//used to dispaly score boards with scores in order
	UPROPERTY(Replicated)
		TArray<struct FTeamScores> m_TeamsScoreSheet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<class UUserWidget> m_HUDWidgetClass;

	UPROPERTY()
		class UUserWidget* m_CurrentWidget;

	UPROPERTY()
		TArray<AActor*> m_CapturePoints;

	UPROPERTY()
		float m_ScoreGoal;

private:

	UFUNCTION()
		void SortTeamScores(int32 low, int32 high);

	UFUNCTION()
		int SortTeamScoresPartition(int32 low, int32 high);
};