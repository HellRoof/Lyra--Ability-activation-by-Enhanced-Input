// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ProgressGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EProgressAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive
};

UCLASS()
class PROGRESS_API UProgressGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EProgressAbilityActivationPolicy ActivationPolicy;

	UFUNCTION()
	void OnAbilityInputReleased(float TimeHeld);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


public:
	FORCEINLINE EProgressAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	
};
