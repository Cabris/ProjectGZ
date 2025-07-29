// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAsset/Input/GZDataAssetInputConfig.h"
#include "InputTriggers.h"
#include "GameplayTagContainer.h"
#include "GZInputComponent.generated.h"

UCLASS()
class PROJECTGZ_API UGZInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template <class UserObject, typename CallbackFunc>
	void BindNativeInputAction(const UGZDataAssetInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject,
	                           CallbackFunc Callback);
};

template <class UserObject, typename CallbackFunc>
void UGZInputComponent::BindNativeInputAction(const UGZDataAssetInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent,
											  UserObject* ContextObject, CallbackFunc Callback)
{
	checkf(InputConfig, TEXT("InputConfig is nullptr"));
	UInputAction* InputAction = InputConfig->FindNativeInputActionByTag(InputTag);
	if (InputAction)
	{
		BindAction(InputAction, TriggerEvent, ContextObject, Callback);
	}
}

