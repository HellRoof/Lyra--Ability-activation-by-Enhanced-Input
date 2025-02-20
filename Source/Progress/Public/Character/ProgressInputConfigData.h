// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "GameplayTagContainer.h"
#include "ProgressInputConfigData.generated.h"

USTRUCT(BlueprintType)
struct FProgressInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;
};

UCLASS(BlueprintType)
class PROGRESS_API UProgressInputConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UProgressInputConfigData(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;

	UFUNCTION(BlueprintCallable)
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FProgressInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FProgressInputAction> AbilityInputActions;
};
