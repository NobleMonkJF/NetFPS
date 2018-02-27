// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "GunBase.h"
#include "Engine.h"
#include "Classes/Camera/CameraComponent.h"


// Sets default values
AFPSCharacter::AFPSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPeronCamera"));
	FirstPersonCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Head"));
	//FirstPersonCameraComponent->SetupAttachment(GetMesh(), );
	// Position the camera a bit above the eyes
	FirstPersonCameraComponent->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	// Allow the pawn to control rotation.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	UAnimInstance *anim = GetMesh()->GetAnimInstance(); *Animation = Cast<UYourAnimInstance>(Mesh->GetAnimInstance());

	m_UsingGun = true;
	AnimBlueprint'/Game/Character/Swat_Animation_BP.Swat_Animation_BP'
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Using FPS Character"));
	}

	const USkeletalMeshSocket* socket = GetMesh()->GetSocketByName(TEXT("gun"));

	if (StartGun && socket)
	{
		Gun = GetWorld()->SpawnActor<AGunBase>(StartGun);
		Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("Gun"));
		Gun->SetActorLocation(GetMesh()->GetSocketLocation(TEXT("Gun")));
		Gun->SetActorRotation(GetMesh()->GetSocketRotation(TEXT("Gun")));
		Gun->InIt(FirstPersonCameraComponent);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Gun->GetAttachParentSocketName().ToString());
	}
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//animation
	m_AnimationDirection = GetActorRotation().Yaw;
	m_AnimationSpeed = FVector::DistSquared(FVector::ZeroVector, GetVelocity());
	m_AnimationSpinPitch = FirstPersonCameraComponent->GetComponentRotation().Pitch * -1;

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::OnJumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::OnJumpStop);

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

void AFPSCharacter::OnJumpStart()
{
	bPressedJump = true;
}

void AFPSCharacter::OnJumpStop()
{
	bPressedJump = false;
}

float AFPSCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	float damage = Super::TakeDamage( DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	m_HealthPoints -= damage;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::SanitizeFloat(m_HealthPoints));

	return damage;
}

void AFPSCharacter::OnUseStart()
{ 
	m_Using = true;

	if (m_UsingGun)
	{
		if (Gun)
			Gun->StartFire();
	}
}

void AFPSCharacter::OnUseStop()
{
	m_Using = false;
	if (m_UsingGun)
	{
		if (Gun)
			Gun->EndFire();
	}
}
