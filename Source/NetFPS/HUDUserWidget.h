// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDUserWidget.generated.h"

/**
 * This class is inherited by the HUD Widget blueprint, So the variables below can be accessed in blueprints and C++
 */
UCLASS()
class NETFPS_API UHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UHUDUserWidget(const FObjectInitializer& ObjectInitializer);
	//UI Vars
	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		float m_UIHealthPercent;
	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		FString m_UIFirstPlaceScore;
	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		FString m_UISecondPlaceScore;
	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		FString m_UIThirdPlaceScore;
	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		FSlateColor m_UIFirstPlaceColour;
	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		FSlateColor m_UISecondPlaceColour;
	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		FSlateColor m_UIThirdPlaceColour;
};
