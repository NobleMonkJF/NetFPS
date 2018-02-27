// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

UCLASS()
class FPSPROJECT_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleDefaultsOnly, category = Projectile)
		USphereComponent* m_CollisionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		UProjectileMovementComponent* m_ProjectileMovement;

	UPROPERTY(EditAnywhere)
		float m_Damage;

	UPROPERTY(EditAnywhere)
		FDamageEvent m_DamageEvent;
public:	
	// Sets default values for this actor's properties
	AProjectileBase(const FObjectInitializer& ObjectInitializer);

	void InItVelocity(const FVector& ShootDirection);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
