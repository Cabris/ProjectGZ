// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GZAimMotionComponent.h"

// Sets default values for this component's properties
UGZAimMotionComponent::UGZAimMotionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGZAimMotionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGZAimMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGZAimMotionComponent:: AimAtRotation(FRotator rotator)
{
	bIsAiming=true;
}

void UGZAimMotionComponent::UnAim()
{
	bIsAiming=false;
}

bool UGZAimMotionComponent::GetIsAiming()
{
	return bIsAiming;
}

