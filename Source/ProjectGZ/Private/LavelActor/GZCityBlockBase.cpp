// Fill out your copyright notice in the Description page of Project Settings.


#include "LavelActor/GZCityBlockBase.h"
#include "PCGComponent.h"
#include "Components/SplineComponent.h"

// Sets default values
AGZCityBlockBase::AGZCityBlockBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	Spline->SetupAttachment(GetRootComponent());
	/*Spline->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(
		EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld, false));*/

	PCG = CreateDefaultSubobject<UPCGComponent>("PCG");
	//PCG->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AGZCityBlockBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGZCityBlockBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
