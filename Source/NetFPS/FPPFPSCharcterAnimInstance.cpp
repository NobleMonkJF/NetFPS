// Fill out your copyright notice in the Description page of Project Settings.

#include "FPPFPSCharcterAnimInstance.h"


UFPPFPSCharcterAnimInstance::UFPPFPSCharcterAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_JumpLand = false;
	m_JumpStart = false;
	m_Speed = 0;
}

