// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfactions/GZAbilitySystemInterface.h"
#include "GZPlayerState.generated.h"
class UGZAbilitySystemComponent;

UCLASS()
class PROJECTGZ_API AGZPlayerState : public APlayerState, public IGZAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGZPlayerState();
	
	FORCEINLINE virtual UGZAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

	FORCEINLINE virtual UGZAttributeSet* GetAttributeSet() const override
	{
		return AttributeSet;
	}

protected:
	UPROPERTY(VisibleAnywhere, Category = "Ability")
	TObjectPtr<UGZAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, Category = "Ability")
	TObjectPtr<UGZAttributeSet> AttributeSet;
};
