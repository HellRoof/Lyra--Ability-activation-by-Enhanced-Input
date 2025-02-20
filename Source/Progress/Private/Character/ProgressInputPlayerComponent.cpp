// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ProgressInputPlayerComponent.h"

UProgressInputPlayerComponent::UProgressInputPlayerComponent()
{
}

void UProgressInputPlayerComponent::AddInputMappings(const UProgressInputConfigData* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	// Here you can handle any custom logic to add something from your input config if required
}

void UProgressInputPlayerComponent::RemoveInputMappings(const UProgressInputConfigData* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UProgressInputPlayerComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
