// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSCharacterAnimInstance.generated.h"

/**
 * This class is inherited by the TPPCharcterAnimation blueprint, So the variables below can be accessed in blueprints and C++
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class NETFPS_API UFPSCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFPSCharacterAnimInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_CameraYRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_AimAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_JumpLand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_JumpStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_IsCrouched;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_IsSprinting;
};
