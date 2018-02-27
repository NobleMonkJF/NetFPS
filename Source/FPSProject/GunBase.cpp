// Fill out your copyright notice in the Description page of Project Settings.

#include "GunBase.h"
#include "CoreMinimal.h"
#include "Engine.h"
#include "ProjectileBase.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"


// Sets default values
AGunBase::AGunBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_GunMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GunMesh"));
	m_GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

// Called when the game starts or when spawned
void AGunBase::BeginPlay()
{
	Super::BeginPlay();

	m_TimeTileChambered = 60 / m_RateOfFire;
	m_TimeTileNextShot = m_TimeTileChambered;
}

// Called every frame
void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_Chambered)
	{
		m_TimeTileChambered -= DeltaTime;

		if (m_TimeTileChambered <= 0)
		{
			m_TimeTileChambered = m_TimeTileNextShot;
			m_Chambered = true;
		}
	}

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

			FRotator rot = (hitPos - m_BulletSpawnPoint->K2_GetComponentLocation()).Rotation();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			AProjectileBase* projectile = world->SpawnActor<AProjectileBase>(m_ProjectileClass, m_BulletSpawnPoint->K2_GetComponentLocation(), rot, SpawnParams);
			if (projectile)
			{
				projectile->InItVelocity(rot.Vector());
				m_Chambered = false;
			}

			m_TriggerJustPressed = false;
		}
	}
}

void AGunBase::StartFire()
{
	m_TriggerJustPressed = true;
	m_TriggerHeldDown = true;
}

void AGunBase::EndFire()
{
	m_TriggerJustPressed = false;
	m_TriggerHeldDown = false;
}

void AGunBase::Reload()
{

}

void AGunBase::Chamber()
{

}

