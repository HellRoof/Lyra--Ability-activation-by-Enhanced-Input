# Lyra--Ability-activation-by-Enhanced-Input
A quick tutorial on how ability binding and activation works from GAS with the Enhanced Input system

## Introduction
I took the idea of figuring out how inputs work in a ***Lyra*** project from ***Epic Games Studio***. This is a simplified implementation without certain plugins, initialization chains, etc. It establishes a direct connection between the Enhanced Input system and the activation of abilities in GAS.  

This project provides enough information to understand the process and use it. However, if you want to explore the full version, you should download and study the ***Lyra*** project. Additionally, knowledge of GAS and the Enhanced Input System is required to completely understand this implementation.  
+ [Lyra Downloading](https://www.fab.com/listings/93faede1-4434-47c0-85f1-bf27c0820ad0)
+ [Lyra Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine)
+ [GAS Documentation](https://github.com/tranek/GASDocumentation)
+ [Enhanced Input System Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)

## About the project
+ The project is named Progress (this is just a random name with no specific reference).  
+ All files include the "Progress" tag in their names.  
+ Developed using UE 5.4. Newer versions are also supported but may require minor code adjustments.  
+ All assets are sourced from Starter Content or the Third-Person Template Project.
+ GAS and Enhanced Input are already enabled and integrated.
+ The project is uploaded using LFS.  

## Document's content
+ [Introduction](#introduction)
+ [About the project](#about-the-project)
+ [Document's content](#document's-content)
+ [Tags](#tags)
+ [Config files](#config-files)
+ [Ability set](#ability-set)
+ [Gameplay ability](#gameplay-ability)
+ [Ability system component](#Ability-system-component)
+ [Game mode and player state](#Game-mode-and-player-state)
+ [Character](#character)
+ [Input player component](#input-player-component)
+ [Input processing component](#input-processing-component)
+ [Player controller](#player-controller)

## Tags
First of all, we need to declare tags that will be used for inputs.  

Macro used: UE_DECLARE_GAMEPLAY_TAG_EXTERN with the tag's name as an argument.  
The ***ProgressGameplayTags.h*** file was created for this purpose.  
```
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
```
<br>

After declaring the tags, we need to define them.  

Macro used: UE_DEFINE_GAMEPLAY_TAG_COMMENT with the tag's name, tag, and description as arguments.  
This is done in the ***ProgressGameplayTags.cpp*** file.  
```
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "Move input.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Crouch, "InputTag.Crouch", "Crouch input.");
```

## Config files
We need to create a structure to store pairs of input actions and input tags.  
(In the project, this structure is called ***FProgressInputAction***)
```
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;
```

<br>

After defining the structure, we need to create a data asset to store input configuration settings.  
Functions ***FindNativeInputActionForTag*** and ***FindAbilityInputActionForTag*** are used for binding inputs and will be needed later.  
Variables ***NativeInputActions*** and ***AbilityInputActions*** are arrays of the structure we created and must be filled.  
This is done in the ***UProgressInputConfigData.h*** file.
```
	UFUNCTION(BlueprintCallable)
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;

	UFUNCTION(BlueprintCallable)
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FProgressInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FProgressInputAction> AbilityInputActions;
```
<br>

Before filling the data asset, we need to create an InputAction and configure it.  
(Example: IA_Move)  
![IA_Move](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/IA_Move.png)  

After implementing the C++ part of the data asset, we need to create it in the editor and fill it.  
(Example: DA_InputConfig)
![DA_InputConfig](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/DA_InputConfig.png)

## Gameplay ability
We need to create an enum class that defines the type of input the ability will handle.  
(In the project, this enum class is called ***EProgressAbilityActivationPolicy***).  
```
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive
```

<br>

Next, we need to override the gameplay ability class to add additional functionality.  
+ ***ActivationPolicy*** – The enum class we created.  
+ ***GetActivationPolicy*** – A function that retrieves the activation policy.  
+ ***ActivateAbility*** – This function must be overridden to trigger the ability task.  
+ ***OnAbilityInputReleased*** – A function called at the end of the ability task.  
This is done in the ***ProgressGameplayAbility.h*** file.
```
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EProgressAbilityActivationPolicy ActivationPolicy;

public:
	FORCEINLINE EProgressAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	UFUNCTION()
	void OnAbilityInputReleased(float TimeHeld);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
```

<br>

The ***ActivateAbility*** function starts the ***UAbilityTask_WaitInputRelease*** task and binds the ***OnAbilityInputReleased*** function to execute when the input is released.  
The ***OnAbilityInputReleased*** function responds to the completion of the ***UAbilityTask_WaitInputRelease*** task and initiates the ability cancellation process.  
This is done in the ***ProgressGameplayAbility.cpp*** file. 
```
void UProgressGameplayAbility::OnAbilityInputReleased(float TimeHeld)
{
    constexpr bool bReplicateEndAbility = true;
    CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility);
}

void UProgressGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);

    if (WaitInputReleaseTask)
    {
        WaitInputReleaseTask->OnRelease.AddDynamic(this, &UProgressGameplayAbility::OnAbilityInputReleased);
        WaitInputReleaseTask->Activate();
    }
}
```

## Ability set
We need to create a structure that stores a TSubclassOf of ***ProgressGameplayAbility*** and its corresponding input tag.  
(In the project, this structure is called ***FProgressAbilitySet_GameplayAbility***).  
```
	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UProgressGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
```

<br>

After defining the structure, we need to create an ability set that stores the abilities to be added to our ASC.  
+ ***GrantedGameplayAbilities*** – An array of structures containing abilities and their corresponding tags.  
To add abilities to the ASC, we need to implement the ***AddToAbilitySystem*** function.  
This is done in the ***ProgressAbilitySet.h*** file. 
```
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FProgressAbilitySet_GameplayAbility> GrantedGameplayAbilities;

public:
	void AddToAbilitySystem(UProgressAbilitySystemComponent* ProgressASC) const;
```

<br>

This is done in the ***ProgressAbilitySet.cpp*** file. 
```
void UProgressAbilitySet::AddToAbilitySystem(UProgressAbilitySystemComponent* ProgressASC) const
{
	if (!IsValid(ProgressASC))
	{
		return;
	}

	if (!ProgressASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FProgressAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		UProgressGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UProgressGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ProgressASC->GiveAbility(AbilitySpec);
	}
}
```

<br>

Once the C++ ability set is defined, we need to create a corresponding data asset in the editor and fill it with abilities and their associated tags.  
(Example: DA_AbilitySet_Main) 
![DA_AbilitySetMain](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/DA_AbilitySet_Main.png)

## Ability system component
We need to override the Ability System Component class to add additional functionality.  
+ ***InputPressedSpecHandles*** – An array of spec handles waiting for execution.  
+ ***InputReleasedSpecHandles*** – An array of spec handles waiting for cancellation.  
+ ***InputHeldSpecHandles*** – An array of spec handles.  
#### Functions Overview
+ ***AbilitySpecInputPressed*** – Activates an ability spec based on the provided spec argument.  
+ ***AbilitySpecInputReleased*** – Cancels an ability spec based on the provided spec argument.  
+ ***AbilityInputTagPressed*** – Adds an ability spec to the ***InputPressedSpecHandles*** array for future activation based on the provided input tag.  
+ ***AbilityInputTagReleased*** – Adds an ability spec to the ***InputReleasedSpecHandles*** array for future cancellation based on the provided input tag.  
+ ***ProcessAbilityInput*** – A handler function that determines which ability specs should be activated and which should be canceled.  
This is done in the ***ProgressAbilitySystemComponent.h*** file.
```
protected:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

public:
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
```

<br>

Full implementation of functions see in the ***ProgressAbilitySystemComponent.cpp*** file.

## Game mode and player state
We need to override the GameMode to configure the ability set.  
(This follows the architecture of the Lyra project, where each GameMode uses a specific ability set.)  
+ ***AbilitySetForCurrentGM*** – An array of abilities assigned to the selected GameMode.  
+ ***GetCurrentGMAbilitySet*** – A function that retrieves ***AbilitySetForCurrentGM***.  
This is done in the ***ProgressGameModeBase.h*** file.  
```
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UProgressAbilitySet>> AbilitySetForCurrentGM;

public:
	FORCEINLINE TArray<TObjectPtr<UProgressAbilitySet>> GetCurrentGMAbilitySet() { return AbilitySetForCurrentGM; }
```

<br>

Once the C++ GameMode is overridden, we need to create its BP version in the editor and assign our ability set.
![GM_AbilitySet](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/GM%20Ability%20set.png)

<br>

We need to override the PlayerState to initialize the ability set.  
+ ***BeginPlay*** – This function must be overridden to include a call to ***SetASCAbilities***.  
+ ***SetASCAbilities*** – Retrieves the ability set from the GameMode and assigns abilities to the ASC.  
This is done in the ***ProgressPlayerState.h*** file.  
```
protected:
	virtual void BeginPlay() override;

public:
	void SetASCAbilities();
```

<br>

Since GameMode exists only on the server, while PlayerState exists on both the server and client, we extract the ability set from the GameMode and apply its abilities to the ASC.  
This is done in the ***ProgressPlayerState.cpp*** file.  
```
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
```

## Input player component
We need to override the EnhancedInputComponent to implement our own functionality.  
We need two functions to help bind movement functions to our abilities:  
+ ***BindNativeAction*** – Binds a native action to an action function.  
+ ***BindAbilityActions*** – Binds an ability action to an ability tag.  
This is done in the ***ProgressInputPlayerComponent.h*** file.  
```
template<class UserClass, typename FuncType>
void BindNativeAction(const UProgressInputConfigData* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void BindAbilityActions(const UProgressInputConfigData* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);


template<class UserClass, typename FuncType>
void UProgressInputPlayerComponent::BindNativeAction(const UProgressInputConfigData* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UProgressInputPlayerComponent::BindAbilityActions(const UProgressInputConfigData* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FProgressInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
```

<br>

Once the C++ ***ProgressInputPlayerComponent*** is created, we need to override the EnhancedInputComponent class in the project settings.  
![DefaultInputComponentClass](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/Default%20Input%20component%20class.png)

## Input processing component
The ***ProgressInputProcessingComponent*** is inherited from the Actor Component and serves as the analog of the Hero Component from ***Lyra***.
It is responsible for processing pressed inputs that trigger abilities associated with specific tags or actions bound to functions.  
+ ***DefaultInputMappings*** – An array of the character's default input mappings.  
+ ***InputConfig*** – The configuration we created and filled earlier.  
+ ***Input_AbilityInputTagPressed*** – This function calls our ASC and invokes the previously created ***AbilityInputTagPressed*** function.  
+ ***Input_AbilityInputTagReleased*** – This function calls our ASC and invokes the previously created ***AbilityInputTagReleased*** function.  
+ ***Input_Move*** and ***Input_Look_Mouse*** – Standard character movement functions.  
(They are used in the project as an example of the combination of the Enhanced Input System with default movement.)  
This is done in the ***ProgressInputProcessingComponent.h*** file. 
```
protected:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look_Mouse(const FInputActionValue& InputActionValue);

public:
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultInputMappings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UProgressInputConfigData> InputConfig;
```

<br>

This is done in the ***ProgressInputProcessingComponent.cpp*** file.  
```
void UProgressInputProcessingComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (AProgressCharacter* Character = GetOwner<AProgressCharacter>())
	{
		if (AProgressPlayerState* CharacterPS = Character->GetPlayerState<AProgressPlayerState>())
		{
			if (UProgressAbilitySystemComponent* ProgressASC = CharacterPS->GetAbilitySystemComponent())
			{
				ProgressASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UProgressInputProcessingComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (AProgressCharacter* Character = GetOwner<AProgressCharacter>())
	{
		if (AProgressPlayerState* CharacterPS = Character->GetPlayerState<AProgressPlayerState>())
		{
			if (UProgressAbilitySystemComponent* ProgressASC = CharacterPS->GetAbilitySystemComponent())
			{
				ProgressASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}
```

<br>

At first, we add all input mappings in a loop.  
Afterward, we bind all abilities with the ***BindAbilityActions*** function.  
The binding for standard movement is done using the ***BindNativeAction*** function.  

Full implementation of functions see in the ***ProgressInputProcessingComponent.cpp*** file.
```
void UProgressInputProcessingComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	if (IsValid(InputConfig))
	{
		for (const TObjectPtr<UInputMappingContext> Mapping : DefaultInputMappings)
		{
			if (UInputMappingContext* IMC = Mapping.Get())
			{
					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;						
					Subsystem->AddMappingContext(IMC, Priority, Options);
			}
		}

		TObjectPtr<UProgressInputPlayerComponent> ProgressPlayerIC = Cast<UProgressInputPlayerComponent>(PlayerInputComponent);
		if (IsValid(ProgressPlayerIC))
		{
			ProgressPlayerIC->AddInputMappings(InputConfig, Subsystem);

			TArray<uint32> BindHandles;
			ProgressPlayerIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

			ProgressPlayerIC->BindNativeAction(InputConfig, ProgressGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
			ProgressPlayerIC->BindNativeAction(InputConfig, ProgressGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_Look_Mouse, /*bLogIfNotFound=*/ false);
		}
	}
}
```
After implementing the C++ part, we need to create the BP version in the editor.  
It will be useful for creating the character.  

## Character
I chose ACharacter for the player because I used the ***Lyra*** project architecture. However, you can also use APawn, in which case you will need to add a ***UFloatingPawnMovement*** component.  
+ ***SetupPlayerInputComponent*** - This is the default character function. It is called once after the input component is initialized.  
This is done in the ***ProgressCharacter.h*** file. 
```
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
```

<br>

In this function, we initialize the bindings for abilities and actions from ***ProgressInputProcessingComponent***.  
This is done in the ***ProgressCharacter.cpp*** file. 
```
void AProgressCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UProgressInputProcessingComponent* ProgressInputComponent = GetComponentByClass<UProgressInputProcessingComponent>(); IsValid(PlayerInputComponent))
	{
		ProgressInputComponent->InitializePlayerInput(PlayerInputComponent);
	}
}
```
Earlier we created a BP version of the ***ProgressInputProcessingComponent*** in the editor. 
We need to add it as component to our character and set it up accordingly.  
![BP_Character ProgressInputProcessing_settings](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/BP_Character%20ProgressInputProcessingComponent%20setting%20up.png)

## Player controller
We need to override the ***PlayerController*** to add functionality for processing inputs.  
+ ***PostProcessInput*** – This post-process function calls the previously created ***ProcessAbilityInput*** function from our ASC.  
+ ***OnUnPossess*** – We need to override this function to decouple our inputs from the ASC when the player is no longer possessed.  
This is done in the ***ProgressPlayerController.h*** file. 
```
protected:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void OnUnPossess() override;
```

<br>

This is done in the ***ProgressPlayerController.cpp*** file. 
```
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
```
