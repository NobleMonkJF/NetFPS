// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"
/*
* This is the main character class, ment to be created into a blueprint where you can a just variables there.
*/

UCLASS()
class NETFPS_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
		void InIt(int32 TeamIndex, int32 CharacterIndex);

	UFUNCTION()
		virtual void BeginPlay() override;

	UFUNCTION()
		virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

#pragma region Input Events
	UFUNCTION()
		void MoveForward(float val);

	UFUNCTION()
		void MoveRight(float val);

	UFUNCTION()
		void OnJumpPress();

	UFUNCTION()
		void OnJumpStart();

	UFUNCTION()
		void OnJumpStop();

	UFUNCTION()
		void OnUseStart();

	UFUNCTION()
		void OnUseStop();

	UFUNCTION()
		void OnCrouchStart();

	UFUNCTION()
		void OnAimStart();

	UFUNCTION()
		void OnAimStop();
#pragma endregion

	UFUNCTION()
		void Landed(const FHitResult & hit) override;

	UPROPERTY(VisibleAnywhere)
		class AGunBase* m_Gun;

	UPROPERTY(VisibleAnywhere, blueprintReadOnly, Category = Camera)
		class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, blueprintReadWrite, Category = Mesh)
		USkeletalMeshComponent* m_FirstPersonMesh;

	//chacters team index in the FPSGameState TeamsArray
	UPROPERTY(Replicated)
		int32 m_CharacterTeamIndex;

	//chacters index in his team
	UPROPERTY(Replicated)
		int32 m_CharacterIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		float m_AnimationSpinPitch;

	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		class UFPSCharacterAnimInstance* m_TPPAnimationInstance;

	UPROPERTY(VisibleAnywhere, blueprintReadOnly)
		class UFPPFPSCharcterAnimInstance* m_FPPAnimationInstance;

private:

	UPROPERTY(EditAnywhere, Replicated)
		float m_HealthPoints;

	UPROPERTY(Replicated)
		FLinearColor m_TeamColor;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AGunBase> m_StartGun;

	const float PLAYER_MAX_HEALTH = 100.0f;

	//is aiming down sights
	UPROPERTY()
		bool m_IsADS;

	//aim down sight completion
	UPROPERTY()
		float m_ADSCompletion;

	UPROPERTY()
		bool m_IsCrouching;

	UPROPERTY()
		float m_CrouchCapsuleHalfHeight;

	UPROPERTY()
		float m_StandingCapsuleHalfHeight;

	UPROPERTY()
		class AFPSGameStateBase* m_GameStateBase;

	//used to set characters team color
	UPROPERTY(VisibleAnywhere)
		UMaterialInstanceDynamic* m_TPPMeshMat;

	//aim down sight time
	UPROPERTY(EditAnywhere)
		float m_ADSTime;

	//is holding a gun
	UPROPERTY(VisibleAnywhere)
		bool m_UsingGun;

	//is using an item (input: left mouse button)
	UPROPERTY(VisibleAnywhere)
		bool m_Using;

	//RPCs
public:

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRPCSpinPitch(float value);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRPCJumpStart();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastRPCJumpStart();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRPCJumpLand();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastRPCJumpLand();
};
