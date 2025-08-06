// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/GZAttributeSet.h"
#include "UObject/Interface.h"
#include "GZAbilitySystemInterface.generated.h"

UINTERFACE()
class UGZAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTGZ_API IGZAbilitySystemInterface
{
	GENERATED_BODY()
public:
	virtual UGZAbilitySystemComponent* GetAbilitySystemComponent() const = 0;
	virtual UGZAttributeSet* GetAttributeSet() const = 0;	
};
