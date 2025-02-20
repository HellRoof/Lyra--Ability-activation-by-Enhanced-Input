// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProgressPlayerController.generated.h"

class UProgressAbilitySystemComponent;

UCLASS()
class PROGRESS_API AProgressPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AProgressPlayerController();

protected:

	UProgressAbilitySystemComponent* GetAbilitySystemComponent() const;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void OnUnPossess() override;
};
