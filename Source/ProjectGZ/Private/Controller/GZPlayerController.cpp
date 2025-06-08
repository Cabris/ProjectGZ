// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/GZPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include <EnhancedInputComponent.h>
#include "InputActionValue.h"

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

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, inputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, inputAxisVector.X);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No pawn controlled"));
	}
}
