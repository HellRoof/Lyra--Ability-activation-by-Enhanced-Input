// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h" 

#include "GameFramework/Character.h"
#include "AbilitySystem/ProgressAbilitySystemComponent.h"

#include "ProgressCharacter.generated.h"

class AProgressPlayerState;

UCLASS()
class PROGRESS_API AProgressCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InitASCAvatarActor();

public:
	AProgressCharacter();

};
