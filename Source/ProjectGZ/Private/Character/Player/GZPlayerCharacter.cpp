// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/GZPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Component/GZAimMotionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectGZ/ProjectGZ.h"

AGZPlayerCharacter::AGZPlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	AimMotionComponent = CreateDefaultSubobject<UGZAimMotionComponent>("AimMotion");
	//AimMotionComponent->RegisterComponent();
	const auto CMC = GetCharacterMovement();
	CMC->bOrientRotationToMovement = true;
	CMC->RotationRate = FRotator(0, DEFAULT_GZ_ROTATE_RATE_YAW, 0);
	CMC->bConstrainToPlane = true;
	CMC->bSnapToPlaneAtStart = true;
	CMC->bUseControllerDesiredRotation = false;
	CMC->GetNavAgentPropertiesRef().bCanCrouch = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;
	Camera->bUsePawnControlRotation = false;
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
	AddControllerPitchInput(AxisValue);
}

void AGZPlayerCharacter::YawCamera(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

UGZAimMotionComponent* AGZPlayerCharacter::GetAimMotionComponent()
{
	return AimMotionComponent.Get();
}

void AGZPlayerCharacter::Strafe()
{
	bIsStrafing=true;
}

void AGZPlayerCharacter::Unstrafe()
{
	bIsStrafing=false;
}

bool AGZPlayerCharacter::IsStrafing()
{
	return bIsStrafing;
}
