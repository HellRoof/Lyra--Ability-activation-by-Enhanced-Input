// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystem/ProgressAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "ProgressPlayerState.generated.h"

class UProgressAbilitySet;

UCLASS()
class PROGRESS_API AProgressPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	TObjectPtr<UProgressAbilitySystemComponent> ASC;

protected:
	virtual void BeginPlay() override;

public:
	AProgressPlayerState();

	virtual UProgressAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void SetASCAbilities();
};
