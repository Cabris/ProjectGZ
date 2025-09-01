// Fill out your copyright notice in the Description page of Project Settings.


#include "Player//GZPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Character/GZCharacterBase.h"
#include "Character/GZAimMotionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfactions/AimControllable.h"
#include "Interfactions/CameraControllable.h"
#include "Interfactions/Strafingable.h"
#include "ProjectGZ/Public/Data/Input/GZDataAssetInputConfig.h"
#include "Game/GZGameplayTags.h"
#include "Game/GZInputComponent.h"
#include "ProjectGZ/ProjectGZ.h"

AGZPlayerController::AGZPlayerController()
{
	bReplicates = true; //for multiplayer usage
	InputConfigDA = nullptr;
}

void AGZPlayerController::BeginPlay()
{
	Super::BeginPlay();
	auto InputSubsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(InputConfigDA);

	if (InputSubsys)
		InputSubsys->AddMappingContext(InputConfigDA->GetInputMappingContext(), 0);

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

	UGZInputComponent* InputCPM = CastChecked<UGZInputComponent>(InputComponent);
	InputCPM->BindNativeInputAction(InputConfigDA, GZGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Move);
	InputCPM->BindNativeInputAction(InputConfigDA, GZGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Look);
	InputCPM->BindNativeInputAction(InputConfigDA, GZGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
	InputCPM->BindNativeInputAction(InputConfigDA, GZGameplayTags::InputTag_Aim, ETriggerEvent::Triggered, this, &ThisClass::Aim);
	InputCPM->BindNativeInputAction(InputConfigDA, GZGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Crouch);
	InputCPM->BindNativeInputAction(InputConfigDA, GZGameplayTags::InputTag_Sprint, ETriggerEvent::Triggered, this, &ThisClass::Sprint);
	InputCPM->BindNativeInputAction(InputConfigDA, GZGameplayTags::InputTag_Strafe, ETriggerEvent::Triggered, this, &ThisClass::Strafe);

	InputCPM->BindAbilityInputActions(InputConfigDA, ETriggerEvent::Triggered, this, &ThisClass::AbilityInputPressed);
	InputCPM->BindAbilityInputActions(InputConfigDA, ETriggerEvent::Completed, this, &ThisClass::AbilityInputReleased);
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
	if (!ControlledPawn) return;
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
		if (!ControlledPawn) return;
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
		if (!ControlledPawn) return;
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

void AGZPlayerController::Sprint(const FInputActionValue& inputActionValue)
{
}

void AGZPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
	UGZAbilitySystemComponent* ASC = GetGZAbilitySystem();
	if (ASC)
	{
		ASC->HandlePostProcessInput(DeltaTime, bGamePaused);
	}
}

void AGZPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	UGZAbilitySystemComponent* ASC = GetGZAbilitySystem();
	if (ASC)
	{
		ASC->HandleAbilityInputPressed(InputTag);
	}
}

void AGZPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	UGZAbilitySystemComponent* ASC = GetGZAbilitySystem();
	if (ASC)
	{
		ASC->HandleAbilityInputReleased(InputTag);
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
	AGZCharacterBase* ControlledPawn = GetPawn<AGZCharacterBase>();
	if (ControlledPawn)
	{
		return ControlledPawn;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No pawn controlled"));
		return nullptr;
	}
}

UGZAbilitySystemComponent* AGZPlayerController::GetGZAbilitySystem() const
{
	AGZCharacterBase* GZCharacter = GetGZCharacter();
	if (!GZCharacter) return nullptr;
	UGZAbilitySystemComponent* ASC = GZCharacter->GetAbilitySystemComponent();
	return ASC;
}
