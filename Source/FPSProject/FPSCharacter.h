// Fill out your copyright notice in the Description page of Project Settings.

class UCameraComponent;
class AGunBase;

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	UFUNCTION()
		void MoveForward(float val);
	UFUNCTION()
		void MoveRight(float val);
	UFUNCTION()
		void OnJumpStart();
	UFUNCTION()
		void OnJumpStop();
	UFUNCTION()
		void OnUseStart();
	UFUNCTION()
		void OnUseStop();


	UPROPERTY(VisibleAnywhere, blueprintReadOnly, Category = Camera)
		UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AGunBase> StartGun;

	UPROPERTY(VisibleAnywhere)
		AGunBase* Gun;

	UPROPERTY(VisibleAnywhere)
		bool m_UsingGun;


	UPROPERTY(VisibleAnywhere)
		bool m_Using;

	UPROPERTY(EditAnywhere)
		float m_HealthPoints;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_AnimationSpinPitch;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_AnimationDirection;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_AnimationSpeed;


	UPROPERTY(VisibleAnywhere)
		UYourAnimInstance m_AnimationInstance;
};
