// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ProgressPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "GameMode/ProgressGameModeBase.h"
#include "Character/ProgressInputProcessingComponent.h"

AProgressPlayerState::AProgressPlayerState()
{
	ASC = CreateDefaultSubobject<UProgressAbilitySystemComponent>(TEXT("ASC"));

	if (IsValid(ASC))
	{
		ASC->SetOwnerActor(this);
	}

	SetReplicates(true);
}

void AProgressPlayerState::BeginPlay()
{
	Super::BeginPlay();

	SetASCAbilities();
}

UProgressAbilitySystemComponent* AProgressPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

void AProgressPlayerState::SetASCAbilities()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	AProgressGameModeBase* CurrentGM = Cast<AProgressGameModeBase>(World->GetAuthGameMode());
	if (!IsValid(CurrentGM))
	{
		return;
	}

	if (CurrentGM->GetCurrentGMAbilitySet().Num() <= 0)
	{
		return;
	}

	for (const TObjectPtr<UProgressAbilitySet> AbilitySet : CurrentGM->GetCurrentGMAbilitySet())
	{
		if (IsValid(AbilitySet.Get()))
		{
			AbilitySet->AddToAbilitySystem(ASC);
		}
	}
}
