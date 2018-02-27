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

class UArrowComponent;
class AProjectileBase;
class UCameraComponent;

UCLASS()
class FPSPROJECT_API AGunBase : public AActor
{
	GENERATED_BODY()
	
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
	UPROPERTY(EditAnywhere)
		int m_BulletCount;
	UPROPERTY(EditAnywhere)
		int m_FullMagBulletCount;
	
	UPROPERTY()
		double m_TimeTileChambered;
	UPROPERTY()
		float m_TimeTileNextShot;

	UPROPERTY(EditAnywhere)
		UArrowComponent* m_BulletSpawnPoint;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_GunMesh;

	UPROPERTY(VisibleAnywhere)
		UCameraComponent* m_Camera;


	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectileBase> m_ProjectileClass;

public:	
	// Sets default values for this actor's properties
	AGunBase(const FObjectInitializer& ObjectInitializer);
	void InIt(UCameraComponent* cam);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void StartFire();
	void EndFire();
	void Reload();
	void Chamber();
};

