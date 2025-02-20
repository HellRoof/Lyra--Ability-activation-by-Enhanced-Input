// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ProgressInputConfigData.h"

UProgressInputConfigData::UProgressInputConfigData(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UProgressInputConfigData::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FProgressInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* UProgressInputConfigData::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FProgressInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}
