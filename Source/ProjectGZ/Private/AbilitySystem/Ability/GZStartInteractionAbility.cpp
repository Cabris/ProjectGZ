#include "AbilitySystem/Ability/GZStartInteractionAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "Interfactions/GZInteractable.h"
#include "AbilitySystem/Ability/GZInteractAbility.h"
#include "Character/GZCharacterBase.h"
#include "ProjectGZ/ProjectGZ.h"

UGZStartInteractionAbility::UGZStartInteractionAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// 這個能力通常是「常駐被動」，可搭配授與時機（如角色初始化）給予
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 設置預設檢測器類別
	DetectorClass = UGZInteractionDetector::StaticClass();

	// 設置預設配置
	DetectorConfig.TraceRadius = 250.f;
	DetectorConfig.TraceDistance = 600.f;
	DetectorConfig.TraceInterval = 0.08f;
	DetectorConfig.TraceChannel = ECC_Visibility;
	DetectorConfig.AimWeight = 0.7f;
	DetectorConfig.DistanceWeight = 0.3f;
	DetectorConfig.bEnableDebugDraw = false;
}

void UGZStartInteractionAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UGZStartInteractionAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	// 清理檢測器
	if (InteractionDetector)
	{
		InteractionDetector->StopDetection();
		InteractionDetector = nullptr;
	}
}

void UGZStartInteractionAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::OnAvatarSet] Invalid ActorInfo or Avatar"));
		return;
	}

	// 清理舊的檢測器
	if (InteractionDetector)
	{
		InteractionDetector->StopDetection();
		InteractionDetector = nullptr;
	}

	// 創建新的檢測器

	if (!IsValid(DetectorClass))
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::OnAvatarSet] DetectorClass is null"));
		return;
	}
	InteractionDetector = NewObject<UGZInteractionDetector>(this, DetectorClass);

	if (!InteractionDetector)
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::OnAvatarSet] Failed to create InteractionDetector"));
		return;
	}

	// 初始化檢測器
	InteractionDetector->Initialize(GetWorld(), ActorInfo->AvatarActor.Get(), DetectorConfig);

	// 綁定焦點變更回調
	InteractionDetector->OnFocusChanged.AddDynamic(this, &UGZStartInteractionAbility::OnFocusChanged);

	// 開始檢測
	InteractionDetector->StartDetection();

	UE_LOG(LogTemp, Log, TEXT("[UGZStartInteractionAbility::OnAvatarSet] InteractionDetector created and started for: %s"),
	       *ActorInfo->AvatarActor->GetName());
}

void UGZStartInteractionAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("[UGZStartInteractionAbility::ActivateAbility]"
		       " Policy=%d IsCDO=%d This=%p Outer=%s"),
	       (int32)GetInstancingPolicy(),
	       HasAnyFlags(RF_ClassDefaultObject),
	       this,
	       *GetOuter()->GetName());

	if (AttemptInteraction_Internal())
	{
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

bool UGZStartInteractionAbility::AttemptInteraction_Internal()
{
	if (!InteractionDetector)
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::AttemptInteraction_Internal] InteractionDetector is null"));
		return false;
	}

	AActor* Target = InteractionDetector->GetCurrentFocusActor();
	if (!Target) return false;
	IGZInteractable* Interactable = Cast<IGZInteractable>(Target);
	if (!Interactable || !Interactable->IsInteractable())
		return false;

	UGZAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	AActor* ASCOwnerActor = ASC->GetOwnerActor();
	AActor* ASCAvatarActor = ASC->GetAvatarActor();

	FGameplayTag InteractionTag = Interactable->GetInteractAbilityTriggerTag();
	FGameplayEventData Evt;
	Evt.EventTag = InteractionTag;
	Evt.Instigator = ASCAvatarActor; // 誰發起
	Evt.Target = Target; // 互動對象
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ASCOwnerActor, InteractionTag, Evt);
	return true;
}

void UGZStartInteractionAbility::OnFocusChanged(AActor* NewFocus, AActor* OldFocus)
{
	if (NewFocus)
	{
		UE_LOG(LogTemp, Log, TEXT("[UGZStartInteractionAbility::OnFocusChanged] New focus: %s"),
		       *NewFocus->GetActorNameOrLabel());
	}

	if (OldFocus)
	{
		UE_LOG(LogTemp, Log, TEXT("[UGZStartInteractionAbility::OnFocusChanged] Old focus: %s"),
		       *OldFocus->GetActorNameOrLabel());
	}
}
