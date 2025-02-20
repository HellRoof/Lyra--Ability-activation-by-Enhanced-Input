// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ProgressAbilitySet.generated.h"

class UProgressAbilitySystemComponent;
class UProgressGameplayAbility;
class UObject;

USTRUCT(BlueprintType)
struct FProgressAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UProgressGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};



UCLASS()
class PROGRESS_API UProgressAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FProgressAbilitySet_GameplayAbility> GrantedGameplayAbilities;

public:
	UProgressAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void AddToAbilitySystem(UProgressAbilitySystemComponent* ProgressASC) const;

};
