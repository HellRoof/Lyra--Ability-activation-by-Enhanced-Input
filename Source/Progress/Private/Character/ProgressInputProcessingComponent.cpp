// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ProgressInputProcessingComponent.h"

#include "ProgressGameplayTags.h"
#include "Character/ProgressCharacter.h"
#include "Character/ProgressInputPlayerComponent.h"
#include "PlayerState/ProgressPlayerState.h"
#include "GameplayTagContainer.h"

UProgressInputProcessingComponent::UProgressInputProcessingComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UProgressInputProcessingComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	if (!IsValid(PlayerInputComponent))
	{
		return;
	}
	
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!IsValid(PC))
	{
		return;
	}

	ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	if (!IsValid(LP))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(Subsystem))
	{
		return;
	}

	Subsystem->ClearAllMappings();

	if (IsValid(InputConfig))
	{
		for (const TObjectPtr<UInputMappingContext> Mapping : DefaultInputMappings)
		{
			if (UInputMappingContext* IMC = Mapping.Get())
			{
					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;
					// Actually add the config to the local player							
					Subsystem->AddMappingContext(IMC, Priority, Options);
			}
		}

		TObjectPtr<UProgressInputPlayerComponent> ProgressPlayerIC = Cast<UProgressInputPlayerComponent>(PlayerInputComponent);
		if (IsValid(ProgressPlayerIC))
		{
			// Add the key mappings that may have been set by the player
			ProgressPlayerIC->AddInputMappings(InputConfig, Subsystem);

			// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
			// be triggered directly by these input actions Triggered events. 
			TArray<uint32> BindHandles;
			ProgressPlayerIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

			ProgressPlayerIC->BindNativeAction(InputConfig, ProgressGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
			ProgressPlayerIC->BindNativeAction(InputConfig, ProgressGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_Look_Mouse, /*bLogIfNotFound=*/ false);
		}
	}
}

void UProgressInputProcessingComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (AProgressCharacter* Character = GetOwner<AProgressCharacter>())
	{
		if (AProgressPlayerState* CharacterPS = Character->GetPlayerState<AProgressPlayerState>())
		{
			if (UProgressAbilitySystemComponent* ProgressASC = CharacterPS->GetAbilitySystemComponent())
			{
				ProgressASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UProgressInputProcessingComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (AProgressCharacter* Character = GetOwner<AProgressCharacter>())
	{
		if (AProgressPlayerState* CharacterPS = Character->GetPlayerState<AProgressPlayerState>())
		{
			if (UProgressAbilitySystemComponent* ProgressASC = CharacterPS->GetAbilitySystemComponent())
			{
				ProgressASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UProgressInputProcessingComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!IsValid(Owner))
	{
		return;
	}

	AController* Controller = Cast<AController>(Owner->GetController());
	if (!IsValid(Controller))
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Owner->AddMovementInput(MovementDirection, Value.X, true);
	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Owner->AddMovementInput(MovementDirection, Value.Y, true);
	}
}

void UProgressInputProcessingComponent::Input_Look_Mouse(const FInputActionValue& InputActionValue)
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!IsValid(Owner))
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Owner->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Owner->AddControllerPitchInput(Value.Y);
	}
}