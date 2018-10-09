// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunBase.generated.h"


UENUM(BlueprintType)
enum class GunType : uint8
{
	BotAction UMETA(DisplayName = "BoltAction"),
	Semiauto  UMETA(DisplayName = "Semi-Auto"),
	FullAuto  UMETA(DisplayName = "Fullauto")
};

// This is the base gun class, it's ment to act like a real fire arm. When creating a blueprint it can be fully ajusted to make the weapon act as desired.
// variables adjustable via blueprint: rate of fire, gunType , reload time, magazine size, projectile and mesh
UCLASS()
class NETFPS_API AGunBase : public AActor
{
	GENERATED_BODY()

public:
	AGunBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
		void InIt(class UCameraComponent* cam);

	UFUNCTION()
		virtual void BeginPlay() override;

	UFUNCTION()
		virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void StartFire();

	UFUNCTION()
		void EndFire();

	UFUNCTION()
		void Reload();

	UFUNCTION()
		void Chamber();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
		GunType m_Type;

	UPROPERTY(EditAnywhere)
		float m_RateOfFire;

	UPROPERTY(VisibleAnywhere)
		bool m_Chambered;

	UPROPERTY(VisibleAnywhere)
		bool m_TriggerJustPressed;

	UPROPERTY(VisibleAnywhere)
		bool m_TriggerHeldDown;

	UPROPERTY(EditAnywhere)
		float m_RelaodTime;

	UPROPERTY(VisibleAnywhere)
		int m_BulletCount;

	UPROPERTY(EditAnywhere)
		int m_FullMagBulletCount;
	
	UPROPERTY()
		double m_TimeTileChambered;

	UPROPERTY()
		float m_TimeTileNextShot;

	UPROPERTY(EditAnywhere)
		class UArrowComponent* m_BulletSpawnPoint;

	UPROPERTY(EditAnywhere)
		class UArrowComponent* m_HoldPoint;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_GunMesh;

	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* m_Camera;

	UPROPERTY(VisibleAnywhere)
		class AFPSCharacter* m_UsingCharacter;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectileBase> m_ProjectileClass;

	//RPCs
public:
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRPCSpawnProjectile(const FVector& pos, const FRotator& rot);
};

