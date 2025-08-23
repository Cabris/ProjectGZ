#include "AbilitySystem/Ability/Combat/GZBowAttackTestAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"


void UGZBowAttackTestAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 進入「蓄力」邏輯
	StartCharging();

	// 等待釋放輸入
	//若 bTestAlreadyReleased = true，當前沒有「按住」的狀態就立刻觸發 OnRelease（返回時間 0）。
	//若 bTestAlreadyReleased = false，它只會註冊釋放事件，等待之後真的收到 release 才觸發。
	UAbilityTask_WaitInputRelease* WaitReleaseTask =
		UAbilityTask_WaitInputRelease::WaitInputRelease(this, /*bTestAlreadyReleased=*/false);

	UAnimMontage* MontageToPlay = nullptr;
	float Rate = 1.f;
	UAbilityTask_PlayMontageAndWait* WaitMontage =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("BOW"), MontageToPlay, Rate);
	WaitMontage->OnCompleted.AddDynamic(this, &UGZBowAttackTestAbility::OnMontageCompleted);
	WaitMontage->ReadyForActivation();

	// 綁定釋放事件
	WaitReleaseTask->OnRelease.AddDynamic(this, &UGZBowAttackTestAbility::OnChargeReleased);
	WaitReleaseTask->ReadyForActivation();
}

void UGZBowAttackTestAbility::StartCharging()
{
	// 開始蓄力的處理（播放動畫、加特效、計時…）
	UE_LOG(LogTemp, Log, TEXT("Bow: Charging started"));
	ChargeStartTime = GetWorld()->GetTimeSeconds();
}

void UGZBowAttackTestAbility::OnChargeReleased(float TimeHeld)
{
	// TimeHeld 是本地記錄的按住時間
	float ChargeDuration = GetWorld()->GetTimeSeconds() - ChargeStartTime;

	UE_LOG(LogTemp, Log, TEXT("Bow: Released after %.2f sec (Task measured: %.2f sec)"), ChargeDuration, TimeHeld);

	// 根據蓄力時間計算射擊威力
	FireArrow(ChargeDuration);

	// 能力結束
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGZBowAttackTestAbility::OnMontageCompleted()
{
	
}

void UGZBowAttackTestAbility::FireArrow(float ChargeTime)
{
	// 這裡生成箭矢、設定初速等
	UE_LOG(LogTemp, Log, TEXT("Arrow fired with charge time %.2f"), ChargeTime);
}
