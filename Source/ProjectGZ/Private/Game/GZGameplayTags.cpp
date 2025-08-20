// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GZGameplayTags.h"

namespace GZGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Crouch, "InputTag.Crouch")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Aim, "InputTag.Aim")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Strafe, "InputTag.Strafe")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interaction, "InputTag.Interaction")

	UE_DEFINE_GAMEPLAY_TAG(MessageTag_Effect_Applied, "MessageTag.Effect.Applied")
	UE_DEFINE_GAMEPLAY_TAG(MessageTag_Inventory_Initialized, "MessageTag.Inventory.Initialized")
	UE_DEFINE_GAMEPLAY_TAG(Item_Stack_Quantity, "Item.Stack.Quantity")
	UE_DEFINE_GAMEPLAY_TAG(Item_Stack_Ammo, "Item.Stack.Ammo")
	UE_DEFINE_GAMEPLAY_TAG(Item_Stack_Ammo_Spare, "Item.Stack.Ammo.Spare")

	UE_DEFINE_GAMEPLAY_TAG(Action_Interaction_Collect, "Action.Interaction.Collect")
	UE_DEFINE_GAMEPLAY_TAG(Action_Interaction_OpenDoor, "Action.Interaction.OpenDoor")
	UE_DEFINE_GAMEPLAY_TAG(Action_Attack_Ranged_Pistol, "Action.Attack.Ranged.Pistol")
	UE_DEFINE_GAMEPLAY_TAG(Action_Attack_Ranged_Rifle, "Action.Attack.Ranged.Rifle")

	UE_DEFINE_GAMEPLAY_TAG(State_Aiming, "State.Aiming")
	UE_DEFINE_GAMEPLAY_TAG(State_Reloading, "State.Reloading")
	UE_DEFINE_GAMEPLAY_TAG(State_Sprinting, "State.Sprinting")

	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Generic, "Cooldown.Generic");


	//UE_DEFINE_GAMEPLAY_TAG(MessageTag_Inventory_Changed, "MessageTag.Inventory.Changed")
}

FGZGameplayTags FGZGameplayTags::Instance;

void FGZGameplayTags::Init()
{
	Instance.Attributes_Vigor_Health = UGameplayTagsManager::Get().AddNativeGameplayTag("Attributes.Vigor.Health", "");
}
