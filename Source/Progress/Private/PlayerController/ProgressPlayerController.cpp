// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/ProgressPlayerController.h"
#include "PlayerState/ProgressPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystem/ProgressAbilitySystemComponent.h"

AProgressPlayerController::AProgressPlayerController()
{
}

UProgressAbilitySystemComponent* AProgressPlayerController::GetAbilitySystemComponent() const
{
	const AProgressPlayerState* PS = GetPlayerState<AProgressPlayerState>();
	return (PS ? PS->GetAbilitySystemComponent() : nullptr);
}

void AProgressPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UProgressAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AProgressPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (UProgressAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->ClearAbilityInput();
	}
}
