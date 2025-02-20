// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ProgressCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/ProgressPlayerState.h"
#include "Character/ProgressInputProcessingComponent.h"

AProgressCharacter::AProgressCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);
}

void AProgressCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitASCAvatarActor();
}

void AProgressCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UProgressInputProcessingComponent* ProgressInputComponent = GetComponentByClass<UProgressInputProcessingComponent>(); IsValid(PlayerInputComponent))
	{
		ProgressInputComponent->InitializePlayerInput(PlayerInputComponent);
	}
}

void AProgressCharacter::InitASCAvatarActor()
{
	if (!HasAuthority())
	{
		return;
	}

	AProgressPlayerState* PS = GetPlayerState<AProgressPlayerState>();

	if (IsValid(PS))
	{
		TObjectPtr <UProgressAbilitySystemComponent> ASC = PS->GetAbilitySystemComponent();

		if (IsValid(ASC))
		{
			ASC->SetAvatarActor(this);
		}
	}
}

