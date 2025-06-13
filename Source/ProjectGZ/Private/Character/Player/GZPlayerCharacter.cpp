// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/GZPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectGZ/ProjectGZ.h"

AGZPlayerCharacter::AGZPlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, DEFAULT_GZ_ROTATE_RATE_YAW, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AGZPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//called on the server
}

void AGZPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//called on the client
}

void AGZPlayerCharacter::PitchCamera(float AxisValue)
{
}

void AGZPlayerCharacter::YawCamera(float AxisValue)
{
}
