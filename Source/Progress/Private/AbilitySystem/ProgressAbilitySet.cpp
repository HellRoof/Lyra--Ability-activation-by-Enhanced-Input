// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ProgressAbilitySet.h"
#include "AbilitySystem/ProgressAbilitySystemComponent.h"
#include "AbilitySystem/ProgressGameplayAbility.h"

UProgressAbilitySet::UProgressAbilitySet(const FObjectInitializer& ObjectInitializer)
{
}

void UProgressAbilitySet::AddToAbilitySystem(UProgressAbilitySystemComponent* ProgressASC) const
{
	if (!IsValid(ProgressASC))
	{
		return;
	}

	if (!ProgressASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FProgressAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		UProgressGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UProgressGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ProgressASC->GiveAbility(AbilitySpec);
	}
}
