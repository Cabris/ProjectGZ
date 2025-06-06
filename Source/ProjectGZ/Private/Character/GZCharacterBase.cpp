// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GZCharacterBase.h"

// Sets default values
AGZCharacterBase::AGZCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGZCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGZCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGZCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

