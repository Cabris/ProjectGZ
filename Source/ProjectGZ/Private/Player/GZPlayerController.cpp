// Fill out your copyright notice in the Description page of Project Settings.


#include "Player//GZPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include <EnhancedInputComponent.h>
#include "InputActionValue.h"
#include "Character/GZCharacterBase.h"
#include "Component/GZAimMotionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfactions/AimControllable.h"
#include "Interfactions/CameraControllable.h"
#include "Interfactions/Strafingable.h"

AGZPlayerController::AGZPlayerController()
{
	bReplicates = true; //for multiplayer usage
}

void AGZPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(GZContext);
	auto inputSubsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(inputSubsys);
	inputSubsys->AddMappingContext(GZContext, 0);
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(true);
	SetInputMode(InputModeData);
}

void AGZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Move);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Jump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Crouch);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Look);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Aim);
	EnhancedInputComponent->BindAction(StrafeAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Strafe);
}

void AGZPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AGZPlayerController::Move(const FInputActionValue& inputActionValue)
{
	FVector2D inputAxisVector = inputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AGZCharacterBase* ControlledPawn = GetGZCharacter();
	if (ControlledPawn)
	{
		ControlledPawn->AddMovementInput(ForwardDirection, inputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, inputAxisVector.X);
	}
}

void AGZPlayerController::Look(const FInputActionValue& inputActionValue)
{
	FVector2D inputAxisVector = inputActionValue.Get<FVector2D>();
	AGZCharacterBase* ControlledPawn = GetGZCharacter();

	ICameraControllable* CameraControllable = Cast<ICameraControllable>(ControlledPawn);
	if (!CameraControllable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn: %s do not implement ICameraControllable."), *ControlledPawn->GetName());
		return;
	}
	CameraControllable->PitchCamera(inputAxisVector.Y);
	CameraControllable->YawCamera(inputAxisVector.X);
}

void AGZPlayerController::Aim(const FInputActionValue& inputActionValue)
{
	bool inputBool = inputActionValue.Get<bool>();
	if (inputBool)
	{
		AGZCharacterBase* ControlledPawn = GetGZCharacter();
		IAimControllable* AimControllable = Cast<IAimControllable>(ControlledPawn);
		if (!AimControllable)
		{
			UE_LOG(LogTemp, Warning, TEXT("Pawn: %s do not implement IAimControllable."), *ControlledPawn->GetName());
			return;
		}
		auto AMC = AimControllable->GetAimMotionComponent();
		if (!AMC)
		{
			UE_LOG(LogTemp, Warning, TEXT("Pawn: %s do not have AimMotionComponent."), *ControlledPawn->GetName());
			return;
		}
		if (!AMC->GetIsAiming())
		{
			FRotator cRotation = GetControlRotation();
			AMC->AimAtRotation(cRotation);
		}
		else
		{
			AMC->UnAim();
		}
	}
}

void AGZPlayerController::Strafe(const FInputActionValue& inputActionValue)
{
	bool inputBool = inputActionValue.Get<bool>();
	if (inputBool)
	{
		AGZCharacterBase* ControlledPawn = GetGZCharacter();
		IStrafingable* Strafingable = Cast<IStrafingable>(ControlledPawn);
		if (Strafingable)
		{
			if (!Strafingable->IsStrafing())
				Strafingable->Strafe();
			else
				Strafingable->Unstrafe();
		}
	}
}

void AGZPlayerController::Jump(const FInputActionValue& inputActionValue)
{
	bool inputBool = inputActionValue.Get<bool>();
	if (inputBool)
	{
		AGZCharacterBase* ControlledPawn = GetGZCharacter();
		if (ControlledPawn)
			ControlledPawn->Jump();
	}
}


void AGZPlayerController::Crouch(const FInputActionValue& inputActionValue)
{
	bool inputBool = inputActionValue.Get<bool>();
	if (inputBool)
	{
		AGZCharacterBase* ControlledPawn = GetGZCharacter();
		if (ControlledPawn)
		{
			// bool CanEverCrouch = ControlledPawn->GetCharacterMovement()->CanEverCrouch();
			// bool IsSimulatingPhysics = ControlledPawn->GetRootComponent()->IsSimulatingPhysics();
			// UE_LOG(LogTemp, Warning, TEXT("CanEverCrouch is %s"), ( CanEverCrouch ? TEXT("true"): TEXT("false") ));
			// UE_LOG(LogTemp, Warning, TEXT("IsSimulatingPhysics is %s"), ( IsSimulatingPhysics ? TEXT("true"): TEXT("false") ));
			const auto CMC = ControlledPawn->GetCharacterMovement();
			if (!CMC->IsCrouching())
				ControlledPawn->Crouch();
			else
				ControlledPawn->UnCrouch();
		}
	}
}

AGZCharacterBase* AGZPlayerController::GetGZCharacter() const
{
	try
	{
		AGZCharacterBase* ControlledPawn = GetPawn<AGZCharacterBase>();
		check(ControlledPawn);
		return ControlledPawn;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Warning, TEXT("No pawn controlled"));
	}
}
