// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "FPSGameStateBase.h"
#include "GunBase.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "Classes/Camera/CameraComponent.h"
#include "FPSCharacterAnimInstance.h"
#include "FPPFPSCharcterAnimInstance.h"
#include "HUDUserWidget.h"


// Sets default values
AFPSCharacter::AFPSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPeronCamera"));
	FirstPersonCameraComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	//// Allow the pawn to control rotation.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	m_FirstPersonMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("FirstPersonMesh"));
	m_FirstPersonMesh->AttachToComponent(FirstPersonCameraComponent, FAttachmentTransformRules::KeepRelativeTransform);

	bReplicates = true;

	m_HealthPoints = 100.0f;
	m_CrouchCapsuleHalfHeight = 44.0f;
	m_StandingCapsuleHalfHeight = 88.0f;

	m_UsingGun = true;
	m_IsCrouching = false;
	m_ADSTime = 2.0f;
	m_IsADS = false;
	m_ADSCompletion = 0;
}

void AFPSCharacter::InIt(int32 TeamIndex, int32 CharacterIndex)
{
	//Setting color
	switch (TeamIndex)
	{
	case 0:
		m_TeamColor = FTeamColors::LC_TEAM_ZERO;
		break;

	case 1:
		m_TeamColor = FTeamColors::LC_TEAM_ONE;
		break;

	default:
		break;
	}

	m_CharacterTeamIndex = TeamIndex;
	m_CharacterIndex = CharacterIndex;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	//spawning Gun
	const USkeletalMeshSocket* socket = GetMesh()->GetSocketByName(TEXT("HoldPoint"));
	if (m_StartGun && socket && Role == ROLE_Authority)
	{
		FActorSpawnParameters params;
		params.Owner = this;

		m_Gun = GetWorld()->SpawnActor<AGunBase>(m_StartGun, params);
	}

	//Setting mesh visibility according to perspective
	if (IsLocallyControlled() && GetMesh() != NULL)
	{
		GetMesh()->SetVisibility(false);
	}
	else if (m_FirstPersonMesh != NULL)
	{
		m_FirstPersonMesh->SetVisibility(false);
	}
	else if (m_FirstPersonMesh == NULL && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FPP Mesh is NULL"));
	}

	//Getting animation class
	if (GetMesh() == NULL && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("TPP mesh not available"));
	}

	else
	{
		m_TPPAnimationInstance = Cast<UFPSCharacterAnimInstance>(GetMesh()->GetAnimInstance());
		//getting TPP mesh material
		m_TPPMeshMat = GetMesh()->CreateDynamicMaterialInstance(0);
	}

	if (m_FirstPersonMesh == NULL && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("FPP mesh not available"));
	}

	else
	{
		m_FPPAnimationInstance = Cast<UFPPFPSCharcterAnimInstance>(m_FirstPersonMesh->GetAnimInstance());
	}

	m_GameStateBase = (AFPSGameStateBase*)GetWorld()->GetGameState();
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//adjusting spin rortaion for all other clients (looking up and down)
	if (IsLocallyControlled() && FirstPersonCameraComponent != NULL)
	{
		ServerRPCSpinPitch(FirstPersonCameraComponent->GetComponentRotation().Pitch * -1);
	}

	///setting animation values
	if (m_TPPAnimationInstance)
	{
		m_TPPAnimationInstance->m_Direction = m_TPPAnimationInstance->CalculateDirection(GetVelocity(), GetActorRotation());
		m_TPPAnimationInstance->m_Speed = GetVelocity().Size();
		m_TPPAnimationInstance->m_CameraYRotation = m_AnimationSpinPitch;
	}

	if (m_FPPAnimationInstance)
		m_FPPAnimationInstance->m_Speed = GetVelocity().Size();

	//Aim down sight aniamtion
	if (m_IsADS && m_ADSCompletion < m_ADSTime)
	{
		m_ADSCompletion += DeltaTime;

		if (m_ADSCompletion > m_ADSTime)
			m_ADSCompletion = m_ADSTime;

		if (m_TPPAnimationInstance)
			m_TPPAnimationInstance->m_AimAlpha = m_ADSCompletion / m_ADSTime;
	}

	else if (m_ADSCompletion > 0 && !m_IsADS)
	{
		m_ADSCompletion -= DeltaTime;

		if (m_ADSCompletion < 0)
			m_ADSCompletion = 0;

		if (m_TPPAnimationInstance)
			m_TPPAnimationInstance->m_AimAlpha = m_ADSCompletion / m_ADSTime;
	}

	//setting team color
	m_TPPMeshMat->SetVectorParameterValue("BodyColor", m_TeamColor);

	//setting UI vars
	if (IsLocallyControlled())
	{
		UHUDUserWidget* userWidget = (UHUDUserWidget*)m_GameStateBase->m_CurrentWidget;
		if (userWidget)
			userWidget->m_UIHealthPercent = m_HealthPoints / 100;
	}
}

#pragma region Input Methods

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::OnJumpPress);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::OnJumpStop);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AFPSCharacter::OnAimStart);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AFPSCharacter::OnAimStop);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSCharacter::OnCrouchStart);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AFPSCharacter::OnUseStart);
	PlayerInputComponent->BindAction("Use", IE_Released, this, &AFPSCharacter::OnUseStop);
}

void AFPSCharacter::MoveForward(float val)
{
	if (Controller != NULL && val != 0.0f)
	{
		//find Forward
		FRotator rotation = Controller->GetControlRotation();
		//Limit Pitch
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			rotation.Pitch = 0.0f;
		}
		//adding movement
		const FVector direction = FRotationMatrix(rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(direction, val);
	}
}

void AFPSCharacter::MoveRight(float val)
{
	if (Controller != NULL && val != 0.0f)
	{
		const FRotator rotation = Controller->GetControlRotation();
		const FVector direction = FRotationMatrix(rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(direction, val);
	}
}

void AFPSCharacter::OnJumpPress()
{
	if (!m_IsCrouching)
	{
		bPressedJump = true;
		ServerRPCJumpStart();
	}

	else
		OnCrouchStart();

}
void AFPSCharacter::OnJumpStart()
{
	bPressedJump = true;
}

void AFPSCharacter::OnJumpStop()
{
	bPressedJump = false;
}

void AFPSCharacter::OnUseStart()
{
	m_Using = true;

	if (m_UsingGun && m_Gun)
	{
		m_Gun->StartFire();
	}
}

void AFPSCharacter::OnUseStop()
{
	m_Using = false;
	if (m_UsingGun && m_Gun)
	{
		m_Gun->EndFire();
	}
}

void AFPSCharacter::OnAimStart()
{
	m_IsADS = true;
}

void AFPSCharacter::OnAimStop()
{
	m_IsADS = false;
}

void AFPSCharacter::OnCrouchStart()
{
	if (GetMovementComponent()->IsMovingOnGround())
	{
		if (!m_IsCrouching)
		{
			m_IsCrouching = true;
			if (m_TPPAnimationInstance)
				m_TPPAnimationInstance->m_IsCrouched = true;
			GetCapsuleComponent()->SetCapsuleHalfHeight(m_CrouchCapsuleHalfHeight);
			FVector transaltion(0.f, 0.0f, m_CrouchCapsuleHalfHeight);
			GetMesh()->AddLocalOffset(transaltion);
		}
		else
		{
			m_IsCrouching = false;
			if (m_TPPAnimationInstance)
				m_TPPAnimationInstance->m_IsCrouched = false;
			GetCapsuleComponent()->SetCapsuleHalfHeight(m_StandingCapsuleHalfHeight);
			FVector transaltion(0.f, 0.0f, -m_CrouchCapsuleHalfHeight);
			GetMesh()->AddLocalOffset(transaltion);
		}
	}
}
#pragma endregion

#pragma region OverRide Events

void AFPSCharacter::Landed(const FHitResult & hit)
{
	Super::Landed(hit);

	ServerRPCJumpLand();
}

float AFPSCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	float damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	m_HealthPoints -= damage;

	if (m_HealthPoints <= 0)
	{
		AFPSGameMode* gameMode = (AFPSGameMode*)GetWorld()->GetAuthGameMode();
		if (gameMode)
		{
			m_HealthPoints = PLAYER_MAX_HEALTH;
			gameMode->RespawnCharacter(this, m_CharacterTeamIndex, m_CharacterIndex);
		}
	}

	return damage;
}
#pragma endregion

#pragma region GamePlay Methods

#pragma endregion

#pragma region RPC & Replication

//variable repication c++
void AFPSCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AFPSCharacter, m_HealthPoints);
	DOREPLIFETIME(AFPSCharacter, m_AnimationSpinPitch);
	DOREPLIFETIME(AFPSCharacter, m_TeamColor);
	DOREPLIFETIME(AFPSCharacter, m_CharacterTeamIndex);
	DOREPLIFETIME(AFPSCharacter, m_CharacterIndex);
}

void AFPSCharacter::ServerRPCSpinPitch_Implementation(float value)
{
	m_AnimationSpinPitch = value;
}

bool AFPSCharacter::ServerRPCSpinPitch_Validate(float value)
{
	return true;
}

void AFPSCharacter::ServerRPCJumpStart_Implementation()
{
	MulticastRPCJumpStart();
}

bool AFPSCharacter::ServerRPCJumpStart_Validate()
{
	return true;
}

void AFPSCharacter::MulticastRPCJumpStart_Implementation()
{
	if (m_TPPAnimationInstance)
	{
		m_TPPAnimationInstance->m_JumpLand = false;
		m_TPPAnimationInstance->m_JumpStart = true;
	}

	if (m_FPPAnimationInstance)
	{
		m_FPPAnimationInstance->m_JumpLand = false;
		m_FPPAnimationInstance->m_JumpStart = true;
	}
}

bool AFPSCharacter::MulticastRPCJumpStart_Validate()
{
	return true;
}

void AFPSCharacter::ServerRPCJumpLand_Implementation()
{
	MulticastRPCJumpLand();
}

bool AFPSCharacter::ServerRPCJumpLand_Validate()
{
	return true;
}

void AFPSCharacter::MulticastRPCJumpLand_Implementation()
{
	if (m_TPPAnimationInstance)
	{
		m_TPPAnimationInstance->m_JumpLand = true;
		m_TPPAnimationInstance->m_JumpStart = false;
	}

	if (m_FPPAnimationInstance)
	{
		m_FPPAnimationInstance->m_JumpLand = true;
		m_FPPAnimationInstance->m_JumpStart = false;
	}
}

bool AFPSCharacter::MulticastRPCJumpLand_Validate()
{
	return true;
}

#pragma endregion