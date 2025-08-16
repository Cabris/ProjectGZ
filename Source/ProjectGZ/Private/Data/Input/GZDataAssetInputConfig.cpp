// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectGZ/Public/Data/Input/GZDataAssetInputConfig.h"

UInputAction* UGZDataAssetInputConfig::FindNativeInputActionByTag(const FGameplayTag& InputTag)const
{
	for (const FInputActionConfig& InputActionConfig : NativeInputActions)
	{
		if (InputActionConfig.InputTag == InputTag && InputActionConfig.InputAction)
			return InputActionConfig.InputAction;
	}
	UE_LOG(LogTemp, Error, TEXT("Can't find InputAction by Tag: %s"), *InputTag.GetTagName().ToString())
	return nullptr;
}
