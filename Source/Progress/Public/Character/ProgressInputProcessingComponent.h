// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputSubsystems.h"

#include "ProgressInputProcessingComponent.generated.h"

class UProgressInputConfigData;
struct FGameplayTag;
struct FInputActionValue;

UCLASS(Blueprintable)
class PROGRESS_API UProgressInputProcessingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look_Mouse(const FInputActionValue& InputActionValue);

public:
	UProgressInputProcessingComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultInputMappings;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UProgressInputConfigData> InputConfig;
	

};
