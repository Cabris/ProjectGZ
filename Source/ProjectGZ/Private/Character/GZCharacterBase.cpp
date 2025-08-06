// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GZCharacterBase.h"

AGZCharacterBase::AGZCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

UGZAbilitySystemComponent* AGZCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
}

UGZAttributeSet* AGZCharacterBase::GetAttributeSet() const
{
	return nullptr;
}
