// Fill out your copyright notice in the Description page of Project Settings.

#include "GunBase.h"
#include "CoreMinimal.h"
#include "Engine.h"
#include "FPSCharacter.h"
#include "ProjectileBase.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"


// Sets default values
AGunBase::AGunBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_HoldPoint = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("HoldPoint"));

	m_GunMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GunMesh"));
	m_GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_GunMesh->AttachToComponent(m_HoldPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_BulletSpawnPoint = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("BulletSpawnPoint"));
	m_BulletSpawnPoint->AttachToComponent(m_GunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_Chambered = true;
	m_TriggerJustPressed = false;
	m_TriggerHeldDown = false;
	m_RateOfFire = 100.0f;
	m_RelaodTime = 2.0f;
	m_FullMagBulletCount = 30;
	m_BulletCount = m_FullMagBulletCount;
	m_Type = GunType::Semiauto;
}

void AGunBase::InIt(UCameraComponent* cam)
{
	m_Camera = cam;
}

void AGunBase::BeginPlay()
{
	Super::BeginPlay();

	m_UsingCharacter = Cast<AFPSCharacter>(GetOwner());

	//Attaching to appropriate hold point
	if (m_UsingCharacter != NULL)
	{
		m_Camera = m_UsingCharacter->FirstPersonCameraComponent;
		m_UsingCharacter->m_Gun = this;

		if (m_UsingCharacter->IsLocallyControlled())
			AttachToComponent(m_UsingCharacter->m_FirstPersonMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HoldPoint"));
		else
			AttachToComponent(m_UsingCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HoldPoint"));
	}

	m_TimeTileChambered = 60 / m_RateOfFire;
	m_TimeTileNextShot = m_TimeTileChambered;
}

// Called every frame
void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//chamber the fire arm
	if (!m_Chambered)
	{
		m_TimeTileChambered -= DeltaTime;

		if (m_TimeTileChambered <= 0)
		{
			m_TimeTileChambered = m_TimeTileNextShot;
			m_Chambered = true;
		}
	}

	//if the player wants to shoot and the gun can shoot, spawn projectile
	else if ((m_Chambered && m_TriggerJustPressed) ||
		(m_Chambered && m_Type == GunType::FullAuto && m_TriggerHeldDown))
	{
		UWorld* world = GetWorld();

		if (m_ProjectileClass != NULL && world != NULL && m_Camera != NULL)
		{
			FHitResult hit;
			FVector hitPos;
			if (!world->LineTraceSingleByChannel(hit, m_Camera->K2_GetComponentLocation() + m_Camera->GetForwardVector() * 30, m_Camera->K2_GetComponentLocation() + m_Camera->GetForwardVector() * 10000, ECollisionChannel::ECC_WorldStatic))
			{
				hitPos = m_Camera->K2_GetComponentLocation() + m_Camera->GetForwardVector() * 10000;
			}

			else
			{
				hitPos = hit.Location;
			}

			ServerRPCSpawnProjectile(m_BulletSpawnPoint->K2_GetComponentLocation(), (hitPos - m_BulletSpawnPoint->K2_GetComponentLocation()).Rotation());

			m_Chambered = false;
		}
	}

	m_TriggerJustPressed = false;
}

//Called by player to pull trigger
void AGunBase::StartFire()
{
	m_TriggerJustPressed = true;
	m_TriggerHeldDown = true;
}

//Called by player to stop pulling trigger
void AGunBase::EndFire()
{
	m_TriggerJustPressed = false;
	m_TriggerHeldDown = false;
}

//Called by player to reload gun
void AGunBase::Reload()
{

}

//Called to chamber if rifle isn't self loading
void AGunBase::Chamber()
{

}

void AGunBase::ServerRPCSpawnProjectile_Implementation(const FVector& pos, const FRotator& rot)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = m_UsingCharacter;
	SpawnParams.Instigator = Instigator;
	AProjectileBase* projectile = GetWorld()->SpawnActor<AProjectileBase>(m_ProjectileClass, pos, rot, SpawnParams);
}

bool AGunBase::ServerRPCSpawnProjectile_Validate(const FVector& pos, const FRotator& rot)
{
	return true;
}

