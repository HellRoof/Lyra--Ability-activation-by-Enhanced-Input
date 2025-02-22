# Lyra--Ability-activation-by-Enhanced-Input
A quick tutorial on how ability binding and activation works from GAS with the Enhanced Input system

## Introduction
I took the idea of figuring out how the inputs work in a ***Lyra*** project from ***Epic Games Studio***.  
This is a simplified implementation without some plugins, initialization chains, etc.  
This is a direct connection between Enhanced Input and activation of abilities from GAS.  

This project will be enough to understand how it works and use it, but if you want to familiarize with the full version of this process, you need to download and familiarize with the ***Lyra*** project.
You will also need knowledge of GAS and Enhanced Input System to understand this project.  
+ [Lyra Downloading](https://www.fab.com/listings/93faede1-4434-47c0-85f1-bf27c0820ad0)
+ [Lyra Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine)
+ [GAS Documentation](https://github.com/tranek/GASDocumentation)
+ [Enhanced Input System Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)

## About the project
+ The project named as Progress (There is no reference or anything like that. Just a random project name).  
+ All files are also named with tag “Progress”.  
+ Used UE5.4 version. Newer versions are also supported but may require minor code changes.  
+ All assets were used from StarterContent or the Third Person Template Project.
+ GAS and Enhanced Input are already enabled and connected.
+ The project uploaded with LFS

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

Used macro: UE_DECLARE_GAMEPLAY_TAG_EXTERN with tag's name as argument.  
The ***ProgressGameplayTags.h*** file was created for this purpose.  
```
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
```
<br>

After declaring tags, we need to define them.  

Used macro: UE_DEFINE_GAMEPLAY_TAG_COMMENT with tag's name, tag, and tag's description as arguments.  
This is done in the ***ProgressGameplayTags.cpp*** file.  
```
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "Move input.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Crouch, "InputTag.Crouch", "Crouch input.");
```

## Config files
We need to create a structure that will store a pair of input action and input tag.  
(***FProgressInputAction*** in the project)
```
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;
```

<br>

After creating the structure, we need to create a data asset that will store the config of the inputs.  
Functions ***FindNativeInputActionForTag*** and ***FindAbilityInputActionForTag*** are used for binding to inputs and will be needed later on.  
Variables NativeInputActions and AbilityInputActions are arrays of the structure we created, which must be filled.  
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

Before we start filling the data asset, we need to create an InputAction and configure it.  
(The example is an IA_Move)  
![IA_Move](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/IA_Move.png)  

After creating the C++ data asset part, we need to create it in the editor and fill it in.  
(The example is a DA_InputConfig)
![DA_InputConfig](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/DA_InputConfig.png)

## Gameplay ability
We need to create an enum class that will explain what type of input the ability will handle.  
(***EProgressAbilityActivationPolicy*** in the project).  
```
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive
```

<br>

We need to override the gameplay ability class to add some functionality.  
***ActivationPolicy*** Our created enum class.  
***GetActivationPolicy*** Get activation policy function.  
***ActivateAbility*** We need to override the activation function to trigger the ability task.  
***OnAbilityInputReleased*** A function that will calls at the end of the ability task.  
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

***ActivateAbility*** function starts ***UAbilityTask_WaitInputRelease*** task and binds the ***OnAbilityInputReleased*** function on its end when the input is released.  
***OnAbilityInputReleased*** OnAbilityInputReleased reflects on the end of the ***UAbilityTask_WaitInputRelease*** task and starts the process of canceling the ability.  
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
We need to create a structure that will store a TSubclassOf of progress gemaplay ability and it's input tag.  
(***FProgressAbilitySet_GameplayAbility*** in the project).  
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

After we have created the structure, we need to create an ability set.  
It will store the abilities that need to be added to our ASC.  
***GrantedGameplayAbilities*** An array of our structures that contains the abilities and their tags.  
To add it to the ASC, we need to create the AddToAbilitySystem function.  
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

Once the C++ ability set is defined we need to create this data asset in the editor and fill it with abilities with their tags.  
(The example is an DA_AbilitySet_Main) 
![DA_AbilitySetMain](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/DA_AbilitySet_Main.png)

## Ability system component
We need to override the ability system component class to add some functionality.  
***InputPressedSpecHandles*** Array of spec handles that are waiting for execution.  
***InputReleasedSpecHandles*** Array of spec handles that are waiting for cancelation.  
***InputHeldSpecHandles*** Array of spec handles.  
***AbilitySpecInputPressed*** The function that activates ability spec by spec argument.  
***AbilitySpecInputReleased*** The function that cancel ability spec by spec argument.  
***AbilityInputTagPressed*** The function adds ability spec to the array InputPressedSpecHandles for further activation the ability by the argument tag.  
***AbilityInputTagReleased*** The function adds the ability spec to the array InputReleasedSpecHandles for further cancel the ability by argument tag.  
***ProcessAbilityInput*** Handler function that will check which ability specs should be activated and which should be canceled.
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
We need to override the gamemode for the setup ability set.  
(I'm explaining the architecture of the ***Lyra*** project, where a certain gamemode uses a certain ability set).  
***AbilitySetForCurrentGM*** Array of abilities to be given out at the selected gamemode.  
***GetCurrentGMAbilitySet*** Get AbilitySetForCurrentGM function.
This is done in the ***ProgressGameModeBase.h*** file.  
```
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UProgressAbilitySet>> AbilitySetForCurrentGM;

public:
	FORCEINLINE TArray<TObjectPtr<UProgressAbilitySet>> GetCurrentGMAbilitySet() { return AbilitySetForCurrentGM; }
```

<br>

Once the C++ gamemode is overridden, we need to create its BP file in the editor and set our ability set
![GM_AbilitySet](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/GM%20Ability%20set.png)

<br>

We need to override the player state for init the ability set.  
***BeginPlay*** We need to override this function and add a SetASCAbilities call to it.  
***SetASCAbilities*** This function will pull the gamemode ability set and add abilities to our ASC.  
This is done in the ***ProgressPlayerState.h*** file.  
```
protected:
	virtual void BeginPlay() override;

public:
	void SetASCAbilities();
```

<br>

Since gamemode exists only on the server, and player state on both server and client, we take out the ability set and add abilities from it to our ASC.  
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
We need to override EnhancedInputComponent for our own implementation.  
We need 2 functions that will help us bind movement functions to our abilities.  
***BindNativeAction*** The function that will bind a native action to an action function.  
***BindAbilityActions*** The function that will bind an ability action to an ability tag.
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

Once the C++ progress input player component is created, we need to overwrite enhanced input component class in project settings.  
![DefaultInputComponentClass](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/Default%20Input%20component%20class.png)

## Input processing component
The input processing component is inherited from the actor component. 
It is the analog of the hero component from ***Lyra***.  
It is responsible for processing pressed inputs that will trigger abilities with related tags or actions with bound functions.  
***DefaultInputMappings*** Array of character's default input mappings.  
***InputConfig*** The config we created, which we filled in earlier.  
***Input_AbilityInputTagPressed*** This function calls our ASC and calls the previously created AbilityInputTagPressed function.  
***Input_AbilityInputTagReleased*** This function calls our ASC and calls the previously created AbilityInputTagReleased function.  
***Input_Move*** and ***Input_Look_Mouse*** Standard movement character's function.  
(They are used in the project for an example of a combination of enhanced input system with default movement)
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

At first we add all input mappings in a loop.  
After that, we need to add bindings to all abilities with the BindAbilityActions function.  
Binding of a usual movement is done with the BindNativeAction function.  

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
It will be useful for creating character.  

## Character
I chose ACharacter for the player because I used the Lyra project architecture, however you can also use APawn, to which you will need to add a UFloatingPawnMovement component.  
***SetupPlayerInputComponent*** Default character function. It is called once after the input component is initialized.  
This is done in the ***ProgressCharacter.h*** file. 
```
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
```

<br>

In this function, we call the initialization of bindings our ability and actions from progress input processing component.  
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

## Player controller
We need to override the player controller to add the functionality to call the processing of inputs.  
***PostProcessInput*** The post process function calls the previously created ProcessAbilityInput function from our ASC.  
***OnUnPossess*** We need to override this function to decouple our inputs from the ASC.  
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
After implementing the C++ part, we need to create the BP version in the editor.  
We need to add progress input processing component BP version to our character and set it up.  
![BP_Character ProgressInputProcessing_settings](https://github.com/HellRoof/Lyra--Ability-activation-by-Enhanced-Input/blob/main/Documentation%20images/BP_Character%20ProgressInputProcessingComponent%20setting%20up.png)
