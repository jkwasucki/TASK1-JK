// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponComponent.h"
#include "Weapon/WeaponInstanceRuntimeSnapshot.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TaskN1Character.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponRuntimeSnapshot, FWeaponInstanceRuntimeSnapshot, Snapshot);

UCLASS(abstract)
class ATaskN1Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category= "Components",meta = (AllowPrivateAccess = "true"))
	UWeaponComponent* WeaponComponent;
	
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;
	
	/** Fire Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* FireAction;
	
	
	/** Fire Mode Change Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* FireModeAction;
	
	
	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
	UFUNCTION()
	void DisplayWeaponVisuals(UWeaponInstance* WeaponInstance);
	
	// DEBUG SNAPSHOTS
	UFUNCTION()
	void SendWeaponRuntimeDebugSnapshot(UWeaponInstance* WeaponInstance);
	UFUNCTION()
	void HandleWeaponDebugSnapshot_Init(UWeaponInstance* WeaponInstance);
public:
	ATaskN1Character();

protected:


	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void HandleWeaponInput(EWeaponInputCommand Command);

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	FOnWeaponRuntimeSnapshot OnWeaponRuntimeSnapshotDelegate;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WeaponMesh;
	
	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

