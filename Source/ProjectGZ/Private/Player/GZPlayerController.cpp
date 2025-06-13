// Fill out your copyright notice in the Description page of Project Settings.


#include "Player//GZPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include <EnhancedInputComponent.h>
#include "InputActionValue.h"
#include "Character/GZCharacterBase.h"

AGZPlayerController::AGZPlayerController()
{
	bReplicates = true; //for multiplayer usage
}

void AGZPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(GZContext);
	auto inputSubsys=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(inputSubsys);
	inputSubsys->AddMappingContext(GZContext,0);
	bShowMouseCursor=false;
	DefaultMouseCursor= EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AGZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Move);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Jump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Crouch);
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
			ControlledPawn->Crouch();
	}
}

AGZCharacterBase* AGZPlayerController::GetGZCharacter()const
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
