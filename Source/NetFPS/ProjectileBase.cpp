// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileBase.h"


// Sets default values
AProjectileBase::AProjectileBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SpherComp"));
	m_CollisionComp->InitSphereRadius(15.0f);
	m_CollisionComp->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	SetRootComponent(m_CollisionComp);

	m_ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	m_ProjectileMovement->UpdatedComponent = m_CollisionComp;

	m_ProjectileMovement->InitialSpeed = 3000.f;
	m_ProjectileMovement->MaxSpeed = 3000.f;
	m_ProjectileMovement->bRotationFollowsVelocity = true;
	m_ProjectileMovement->bShouldBounce = true;
	m_ProjectileMovement->Bounciness = 0.3f;

	m_DamageEvent =  FDamageEvent();
}

void AProjectileBase::InItVelocity(const FVector& ShootDirection)
{
	if (m_ProjectileMovement)
	{
		m_ProjectileMovement->Velocity = ShootDirection * m_ProjectileMovement->InitialSpeed;
	}
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (OtherComponent->ComponentHasTag(TEXT("player")))
	{
		ServerRPCGiveDamage(OtherActor);
	}
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	Destroy();
}

void AProjectileBase::ServerRPCGiveDamage_Implementation(AActor* OtherActor)
{
	OtherActor->TakeDamage(m_Damage, m_DamageEvent, OtherActor->GetInstigatorController(), this);
}

bool AProjectileBase::ServerRPCGiveDamage_Validate(AActor* OtherActor)
{
	return true;
}

