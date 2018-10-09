// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameStateBase.h"
#include "GunBase.h"
#include "FPSCharacter.h"
#include "Engine.h"
#include "FPSGameMode.h"
#include "MyStructsAndEnums.h"
#include "UnrealNetwork.h"
#include "Blueprint//UserWidget.h"
#include "HUDUserWidget.h"
#include "CapturePointVolume.h"


AFPSGameStateBase::AFPSGameStateBase(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	m_TeamMaxSize = 5;
	m_MaxTeams = 2;

	
	m_Teams.Reserve(m_MaxTeams);
	m_TeamsScoreSheet.Reserve(m_MaxTeams);
	//initializing array of teams
	for (int t = 0; t < m_MaxTeams; t++)
	{
		FMyTeamStruct tempTeam;
		tempTeam.m_Players.Reserve(m_TeamMaxSize);
		//initializing array of players
		for (int p = 0; p < m_TeamMaxSize; p++)
		{
			tempTeam.m_Players.Add(nullptr);
		}

		m_Teams.Add(tempTeam);

		//initializing array of scores
		FTeamScores tempScore;
		tempScore.m_Score = tempTeam.m_TeamScore;
		tempScore.m_TeamIndex = t;
		m_TeamsScoreSheet.Add(tempScore);
	}
}

void AFPSGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	//setting hud
	ChangeWidget(m_HUDWidgetClass);

	//getting all the capture points
	for (TActorIterator<ACapturePointVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->m_CapturePointIndex = m_CapturePoints.Add(*ActorItr);
	}

	//setting the number of players inside capture points to zero for every team
	for (int c = 0; c < m_CapturePoints.Num(); c++)
	{
		for (int t = 0; t < m_Teams.Num(); t++)
		{
			m_Teams[t].m_NumberOfPlayersInsideCapturePoint.Add(0);
		}
	}

	//setting team colours
	for (int t = 0; t < m_Teams.Num(); t++)
	{
		switch (t)
		{
		case 0:
			m_Teams[t].m_TeamColour = FTeamColors::LC_TEAM_ZERO;
			break;

		case 1:
			m_Teams[t].m_TeamColour = FTeamColors::LC_TEAM_ONE;
			break;

		default:
			break;
		}
	}
}

void AFPSGameStateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if this is the server, handle points.
	if (Role == ENetRole::ROLE_Authority)
	{
		int32 playersHoldingCapturePoint = 0;
		int32 teamHoldingPoint = -1;
		bool capturePointContested = false;

		//check how many points are controlled by each team
		for (int c = 0; c < m_CapturePoints.Num(); c++)
		{
			for (int t = 0; t < m_Teams.Num(); t++)
			{
				//if no team is holding point
				if (teamHoldingPoint == -1)
				{
					if (m_Teams[t].m_NumberOfPlayersInsideCapturePoint[c] > playersHoldingCapturePoint)
					{
						playersHoldingCapturePoint = m_Teams[t].m_NumberOfPlayersInsideCapturePoint[c];
						teamHoldingPoint = t;
					}
					continue;
				}

				//if the amout of players are equal at point
				else if (m_Teams[t].m_NumberOfPlayersInsideCapturePoint[c] == m_Teams[teamHoldingPoint].m_NumberOfPlayersInsideCapturePoint[c])
					capturePointContested = true;

				//if you have more players at point
				else if (m_Teams[t].m_NumberOfPlayersInsideCapturePoint[c] > m_Teams[teamHoldingPoint].m_NumberOfPlayersInsideCapturePoint[c])
				{
					capturePointContested = false;
					teamHoldingPoint = t;
					playersHoldingCapturePoint = m_Teams[t].m_NumberOfPlayersInsideCapturePoint[c];
				}
			}

			//give team points
			if (!capturePointContested && teamHoldingPoint >= 0)
			{
				m_Teams[teamHoldingPoint].m_TeamScore += DeltaTime;
			}

			playersHoldingCapturePoint = 0;
			teamHoldingPoint = -1;
			capturePointContested = false;
		}
		//check if someone won and set score array
		for (int t = 0; t < m_Teams.Num(); t++)
		{
			m_TeamsScoreSheet[t].m_Score = m_Teams[t].m_TeamScore;
			m_TeamsScoreSheet[t].m_TeamIndex = t;
			if (m_Teams[t].m_TeamScore >= m_ScoreGoal)
			{
				FString teamNumberScore;
				teamNumberScore.AppendInt(t);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, teamNumberScore + TEXT(" Won"));
			}
		}

		SortTeamScores(0, m_TeamsScoreSheet.Num() - 1);
	}

	//setting IU Score
	UHUDUserWidget* userWidget = (UHUDUserWidget*)m_CurrentWidget;

	if (userWidget)
	{
		for (int t = 0; t < m_TeamsScoreSheet.Num(); t++)
		{
			FString score;
			switch (t)
			{
			case 0:

				score.AppendInt(m_TeamsScoreSheet[t].m_Score);
				userWidget->m_UIFirstPlaceScore = TEXT("1# ") + score;
				userWidget->m_UIFirstPlaceColour = m_Teams[m_TeamsScoreSheet[t].m_TeamIndex].m_TeamColour;
				break;

			case 1:
				score.AppendInt(m_TeamsScoreSheet[t].m_Score);
				userWidget->m_UISecondPlaceScore = TEXT("2# ") + score;
				userWidget->m_UISecondPlaceColour = m_Teams[m_TeamsScoreSheet[t].m_TeamIndex].m_TeamColour;
				break;

			case 2:
				score.AppendInt(m_TeamsScoreSheet[t].m_Score);
				userWidget->m_UIThirdPlaceScore = TEXT("3# ") + score;
				userWidget->m_UIThirdPlaceColour = m_Teams[m_TeamsScoreSheet[t].m_TeamIndex].m_TeamColour;
				break;
			default:
				break;
			}
		}
	}
}

void AFPSGameStateBase::ChangeWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (m_CurrentWidget != nullptr)
	{
		m_CurrentWidget->RemoveFromViewport();
		m_CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		m_CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (m_CurrentWidget != nullptr)
		{
			m_CurrentWidget->AddToViewport();
		}
	}
}

void AFPSGameStateBase::PlayerEnterCapturePoint(int32 TeamIndex, int32 CapturePointIndex)
{
	m_Teams[TeamIndex].m_NumberOfPlayersInsideCapturePoint[CapturePointIndex]++;
}

void AFPSGameStateBase::PlayerExitCapturePoint(int32 TeamIndex, int32 CapturePointIndex)
{
	m_Teams[TeamIndex].m_NumberOfPlayersInsideCapturePoint[CapturePointIndex]--;
}


/* The main function that implements QuickSort
  low  --> Starting index,
  high  --> Ending index */
void AFPSGameStateBase::SortTeamScores(int32 low, int32 high)
{
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now
		   at right place */
		int32 pi = SortTeamScoresPartition(low, high);

		// Separately sort elements before 
		// partition and after partition 
		SortTeamScores(low, pi - 1);
		SortTeamScores(pi + 1, high);
	}
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
	array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int AFPSGameStateBase::SortTeamScoresPartition(int32 low, int32 high)
{
	float pivot = m_TeamsScoreSheet[high].m_Score;    // pivot 
	int32 i = (low - 1);  // Index of smaller element 

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than or 
		// equal to pivot
		if (m_TeamsScoreSheet[j].m_Score >= pivot)
		{
			i++;    // increment index of smaller element 
			//swap
			m_TeamsScoreSheet.Swap(i, j);
		}
	}
	m_TeamsScoreSheet.Swap(i + 1, high);
	return (i + 1);
}

void AFPSGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AFPSGameStateBase, m_Teams);
	DOREPLIFETIME(AFPSGameStateBase, m_TeamsScoreSheet);
}

