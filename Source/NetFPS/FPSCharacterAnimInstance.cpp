// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacterAnimInstance.h"


UFPSCharacterAnimInstance::UFPSCharacterAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//set any default values for your variables here
	m_Speed = 0;
	m_Direction = 0;
	m_CameraYRotation = 0;
	m_AimAlpha = 0;
	m_JumpLand = false;
	m_JumpLand = false;
	m_IsCrouched = false;
	m_IsSprinting = false;

}

