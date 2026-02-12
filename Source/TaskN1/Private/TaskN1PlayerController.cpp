// Copyright Epic Games, Inc. All Rights Reserved.


#include "Public/TaskN1PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Public/TaskN1CameraManager.h"
#include "Blueprint/UserWidget.h"
#include "Public/TaskN1.h"
#include "Widgets/Input/SVirtualJoystick.h"

ATaskN1PlayerController::ATaskN1PlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = ATaskN1CameraManager::StaticClass();
}

void ATaskN1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogTaskN1, Error, TEXT("Could not spawn mobile controls widget."));

		}
	}
	
	// Ideally HUD Component
	if (WeaponDebugWidgetClass)
	{
		if (UWeaponDebugWidget* WeaponDebugWidget = CreateWidget<UWeaponDebugWidget>(this,WeaponDebugWidgetClass))
		{
			WeaponDebugWidget->AddToViewport();
			WeaponDebugWidget->SetVisibility(ESlateVisibility::Hidden);
			
			WeaponDebugWidget->Init(this);
		}
	}
}

void ATaskN1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}
