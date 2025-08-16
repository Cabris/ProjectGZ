// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace GZGameplayTags
{
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move)
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look)
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump)
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint)
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch)
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Aim)
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Strafe)
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Interaction)

	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MessageTag_Effect_Applied)
	PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MessageTag_Inventory_Changed)
	//PROJECTGZ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MessageTag_Inventory_Initialized)
}

struct FGZGameplayTags
{
public:
	static FGZGameplayTags& Get() { return Instance; }
	static void Init();
	FGameplayTag Attributes_Vigor_Health;

private:
	static FGZGameplayTags Instance;
};
