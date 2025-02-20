// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ProgressGameplayAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

void UProgressGameplayAbility::OnAbilityInputReleased(float TimeHeld)
{
    constexpr bool bReplicateEndAbility = true;
    CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility);
}

void UProgressGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);

    if (WaitInputReleaseTask)
    {
        WaitInputReleaseTask->OnRelease.AddDynamic(this, &UProgressGameplayAbility::OnAbilityInputReleased);
        WaitInputReleaseTask->Activate();
    }
}
