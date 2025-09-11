#include "Character/GZAimMotionComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGZ/ProjectGZ.h"

UGZAimMotionComponent::UGZAimMotionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	CurrentActivateCameraPoseRecoveryTime = 0;
	bIsAiming = false;
	RecoveryTimeerHandle.Invalidate();
}

// Called when the game starts
void UGZAimMotionComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentActivateCameraRigTag = DefaultCameraRigTag;
	OnCameraRigTagChanged.Broadcast(CurrentActivateCameraRigTag);
}

void UGZAimMotionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bIsAiming);
}

void UGZAimMotionComponent::SetCameraRigTag(FGameplayTag CameraPoseOverrideTag, float CameraPoseRecoveryTime, bool IsActivate)
{
	FTimerManager& WorldTimerManager = GetOwner()->GetWorldTimerManager();
	//Clear previous 
	if (RecoveryTimeerHandle.IsValid())
	{
		WorldTimerManager.ClearTimer(RecoveryTimeerHandle);
		RecoveryTimeerHandle.Invalidate();
	}

	if (IsActivate)
	{
		CurrentActivateCameraRigTag = CameraPoseOverrideTag;
		CurrentActivateCameraPoseRecoveryTime = CameraPoseRecoveryTime;
	}
	else
	{
		//wait CurrentActivateCameraPoseRecoveryTime
		WorldTimerManager.SetTimer(RecoveryTimeerHandle, this, &ThisClass::OnCameraRecovery,
		                           CurrentActivateCameraPoseRecoveryTime, false);
	}
}

void UGZAimMotionComponent::OnCameraRecovery()
{
	CurrentActivateCameraRigTag = DefaultCameraRigTag;
	OnCameraRigTagChanged.Broadcast(CurrentActivateCameraRigTag);
}

void UGZAimMotionComponent::OnRep_bIsAiming(const bool& bOldIsAiming)
{
}

void UGZAimMotionComponent::AimAtRotation(FRotator rotator)
{
	bIsAiming = true;
}

void UGZAimMotionComponent::UnAim()
{
	bIsAiming = false;
}

bool UGZAimMotionComponent::GetIsAiming()
{
	return bIsAiming;
}
