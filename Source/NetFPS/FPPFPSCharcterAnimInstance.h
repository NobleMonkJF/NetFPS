// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPPFPSCharcterAnimInstance.generated.h"

/**
 * This class is inherited by the FPPCharcterAnimation blueprint, So the variables below can be accessed in blueprints and C++
 */
UCLASS()
class NETFPS_API UFPPFPSCharcterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UFPPFPSCharcterAnimInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_JumpLand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_JumpStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Speed;
};
