// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AbilitySystem/ProgressAbilitySet.h"
#include "ProgressGameModeBase.generated.h"

UCLASS(Blueprintable)
class PROGRESS_API AProgressGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UProgressAbilitySet>> AbilitySetForCurrentGM;

public:
	FORCEINLINE TArray<TObjectPtr<UProgressAbilitySet>> GetCurrentGMAbilitySet() { return AbilitySetForCurrentGM; }
};
