// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDUserWidget.h"



UHUDUserWidget::UHUDUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_UIHealthPercent = 100.0f;
	m_UIFirstPlaceScore = "";
	m_UISecondPlaceScore = "";
	m_UIThirdPlaceScore = "";
	m_UIFirstPlaceColour = FSlateColor::FSlateColor();
	m_UISecondPlaceColour = FSlateColor::FSlateColor();
	m_UIThirdPlaceColour = FSlateColor::FSlateColor();
}
