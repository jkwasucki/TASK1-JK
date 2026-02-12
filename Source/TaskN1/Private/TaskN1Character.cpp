// Copyright Epic Games, Inc. All Rights Reserved.

#include "Public/TaskN1Character.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/TaskN1.h"
#include "Public/Weapon/WeaponComponent.h"
#include "Public/Weapon/WeaponInstanceRuntimeSnapshot.h"

void ATaskN1Character::DisplayWeaponVisuals(UWeaponInstance* WeaponInstance)
{
	if (!IsValid(WeaponInstance)) return;
	UStaticMesh* FoundMesh = WeaponInstance->GetWeaponDefinition()->Mesh.LoadSynchronous();
	if (!FoundMesh) return;
	WeaponMesh->SetStaticMesh(FoundMesh);
}

void ATaskN1Character::SendWeaponRuntimeDebugSnapshot(UWeaponInstance* WeaponInstance)
{
	if (!IsValid(WeaponInstance)) return;
	
	FWeaponInstanceRuntimeSnapshot RuntimeSnapshot;
	RuntimeSnapshot.InstanceID = WeaponInstance->GetInstanceID();
	RuntimeSnapshot.WeaponState = WeaponInstance->GetWeaponState();
	
	UWeaponDefinition* Definition = WeaponInstance->GetWeaponDefinition().Get();
	RuntimeSnapshot.bDefinitionExists = Definition ? true : false;
	RuntimeSnapshot.Definition = Definition;
	
	APawn* Pawn = WeaponInstance->GetOwner().Get();
	RuntimeSnapshot.bOwnerExists = Pawn ? true : false;
	RuntimeSnapshot.WeaponFireMode = WeaponInstance->GetFireMode();
	
	OnWeaponRuntimeSnapshotDelegate.Broadcast(RuntimeSnapshot);
}

void ATaskN1Character::HandleWeaponDebugSnapshot_Init(UWeaponInstance* WeaponInstance)
{
	if (!IsValid(WeaponInstance)) return;
	
	SendWeaponRuntimeDebugSnapshot(WeaponInstance);
	WeaponInstance->OnWeaponRuntimeUpdatedDelegate.RemoveDynamic(this,&ATaskN1Character::SendWeaponRuntimeDebugSnapshot);
	WeaponInstance->OnWeaponRuntimeUpdatedDelegate.AddDynamic(this,&ATaskN1Character::SendWeaponRuntimeDebugSnapshot);
}



ATaskN1Character::ATaskN1Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>("WeaponComponent");
	WeaponComponent->SetComponentTickEnabled(true);
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(FirstPersonCameraComponent);
}

void ATaskN1Character::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponComponent)
	{
		WeaponComponent->OnWeaponGrantedDelegate.AddDynamic(this,&ATaskN1Character::DisplayWeaponVisuals);
		WeaponComponent->OnWeaponGrantedDelegate.AddDynamic(this,&ATaskN1Character::HandleWeaponDebugSnapshot_Init);
	}
}



void ATaskN1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATaskN1Character::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATaskN1Character::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATaskN1Character::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATaskN1Character::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATaskN1Character::LookInput);
	
		// Firing (Weapon)
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATaskN1Character::HandleWeaponInput, EWeaponInputCommand::FirePressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATaskN1Character::HandleWeaponInput, EWeaponInputCommand::FireReleased);
		
		// Fire Mode Change (Weapon)
		EnhancedInputComponent->BindAction(FireModeAction, ETriggerEvent::Started, this, &ATaskN1Character::HandleWeaponInput, EWeaponInputCommand::FireModeChanged);
		
	}
	else
	{
		UE_LOG(LogTaskN1, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}



void ATaskN1Character::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ATaskN1Character::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ATaskN1Character::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATaskN1Character::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ATaskN1Character::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ATaskN1Character::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void ATaskN1Character::HandleWeaponInput(EWeaponInputCommand Command)
{
	
	switch (Command)
	{
		case EWeaponInputCommand::FirePressed:
		{
			WeaponComponent->OnFirePressed();
			break;
		}
		case EWeaponInputCommand::FireReleased:
		{
			WeaponComponent->OnFireReleased();
			break;
		}
		case EWeaponInputCommand::FireModeChanged:
		{
			WeaponComponent->CycleFireMode();
			break;
		}
	}
}
