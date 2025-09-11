#include "AbilitySystem/Ability/GZStartInteractionAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "Interfactions/GZInteractable.h"
#include "AbilitySystem/Ability/GZInteractAbility.h"
#include "Character/GZCharacterBase.h"
#include "Character/GZPawnFeatureComponent.h"
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

void UGZStartInteractionAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	// 清理檢測器
	if (InteractionDetector)
	{
		InteractionDetector->StopDetection();
		InteractionDetector = nullptr;
	}
}

void UGZStartInteractionAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilitySpec& Spec)
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

	//Only Client needs to tracking focusing Items and do the UI broadcast things. 
	if (IsLocalControlled())
	{
		// 綁定焦點變更
		InteractionDetector->OnFocusChanged.AddDynamic(this, &UGZStartInteractionAbility::OnFocusChanged);
		// 開始檢測
		InteractionDetector->StartDetection();
		UE_LOG(LogTemp, Log,
		       TEXT("[UGZStartInteractionAbility::OnAvatarSet] InteractionDetector created and started for: %s"),
		       *ActorInfo->AvatarActor->GetName());
	}
}

void UGZStartInteractionAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UGZAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	//On Dedicated Server or Listen Server, but not On Client
	if (IsNetAuthority() && !IsLocalControlled()||IsListenServer()) 
	{
		//Server Side check
		//Bind delegate for receive TargetData from Client version UGZRangeAttackAbility
		FGameplayAbilitySpecHandle SpecHandle = GetCurrentAbilitySpecHandle();
		FPredictionKey OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
		FAbilityTargetDataSetDelegate& Delegate = ASC->AbilityTargetDataSetDelegate(SpecHandle, OriginalPredictionKey);
		TargetDataSetDelegateHandle = Delegate.AddUObject(this, &ThisClass::OnReceivedTargetDataFromClient);
		//ensure that TargetData received before Bind delegate, return is CalledDelegate
		ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, OriginalPredictionKey);
	}
	
	//On Client or Listen Server or Single Player 
	if (IsLocalControlled()||IsListenServer()) 
	{
		//Local Prediction
		FScopedPredictionWindow PredictionWindow(ASC, true); //開始預測區域
		if (!InteractionDetector)
		{
			Debug::Print(TEXT("[UGZStartInteractionAbility::AttemptInteraction_Internal] InteractionDetector is null"));
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}

		AActor* Target = InteractionDetector->GetCurrentFocusActor();
		if (AttemptInteraction_Internal(Target))
		{
			ClientSendTargetDataToServer(Target);
			CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			Debug::Print(TEXT("[UGZStartInteractionAbility::AttemptInteraction_Internal] AttemptInteraction_Internal success"));
		}
		else
		{
			Debug::Print(TEXT("[UGZStartInteractionAbility::AttemptInteraction_Internal] AttemptInteraction_Internal failed"));
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		}
	}
}

void UGZStartInteractionAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                            bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (TargetDataSetDelegateHandle.IsValid())
	{
		FGameplayAbilitySpecHandle SpecHandle = GetCurrentAbilitySpecHandle();
		FPredictionKey OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
		auto& Delegate = GetAbilitySystemComponent()->AbilityTargetDataSetDelegate(SpecHandle, OriginalPredictionKey);
		Delegate.Remove(TargetDataSetDelegateHandle);
	}
	UE_LOG(LogTemp, Warning, TEXT("[UGZStartInteractionAbility::EndAbility] IsLocalControlled=%d"), IsLocalControlled());
}

bool UGZStartInteractionAbility::AttemptInteraction_Internal(AActor* Target) const
{
	if (!Target)
		return false;
	const IGZInteractable* Interactable = Cast<IGZInteractable>(Target);
	if (!Interactable || !Interactable->IsInteractable())
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("[UGZStartInteractionAbility::AttemptInteraction_Internal] Target is not Interactable, IsLocalControlled=%d"),
		       IsLocalControlled());
		return false;
	}

	UGZAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	AActor* ASCOwnerActor = ASC->GetOwnerActor();
	AActor* ASCAvatarActor = ASC->GetAvatarActor();

	//activating concrete Interaction GA
	FGameplayTag InteractionTag = Interactable->GetInteractAbilityTriggerTag();
	FGameplayEventData EventData;
	EventData.EventTag = InteractionTag;
	EventData.Instigator = ASCAvatarActor; // 誰發起
	EventData.Target = Target; // 互動對象
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ASCOwnerActor, InteractionTag, EventData);
	IGZInteractable::Execute_DoInteract(Target);
	return true;
}

void UGZStartInteractionAbility::ClientSendTargetDataToServer(AActor* Target)
{
	FGameplayAbilityTargetData_ActorArray* Data = new FGameplayAbilityTargetData_ActorArray();
	Data->TargetActorArray.Add(Target);
	FGameplayAbilityTargetDataHandle TargetDataHandle(Data);
	auto OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
	auto ASC = GetAbilitySystemComponent();
	//Send Target Data to Server
	FGameplayTag ApplicationTag;
	ASC->ServerSetReplicatedTargetData(
		GetCurrentAbilitySpecHandle(),
		OriginalPredictionKey,
		TargetDataHandle,
		ApplicationTag,
		ASC->ScopedPredictionKey);
}

void UGZStartInteractionAbility::OnReceivedTargetDataFromClient(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                                FGameplayTag GameplayTag)
{
	if (TargetDataHandle.Data.Num() == 0)
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::OnReceivedTargetDataFromClient] TargetDataHandle.Data is invalid"));
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	FGameplayAbilityTargetData* Data = TargetDataHandle.Data[0].Get();
	if (Data && Data->GetActors().Num() == 0)
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::OnReceivedTargetDataFromClient] Data is invalid"));
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	AActor* TargetFromClient = Data->GetActors()[0].Get();

	if (ServerValidateTargetData(TargetFromClient))
	{
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	Debug::Print(TEXT("[UGZStartInteractionAbility::OnReceivedTargetDataFromClient] ServerValidateTargetData Failed"));
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

bool UGZStartInteractionAbility::ServerValidateTargetData(AActor* TargetFromClient) const
{
	if (!TargetFromClient)
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::ServerValidateTargetData] TargetFromClient is null"));
		return false;
	}

	//Check Collect item is valid
	if (!InteractionDetector)
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::ServerValidateTargetData] InteractionDetector is null"));
		return false;
	}

	InteractionDetector->ForceUpdate();
	AActor* TargetFromServer = InteractionDetector->GetCurrentFocusActor();
	if (!TargetFromServer)
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::ServerValidateTargetData] TargetFromServer is null"));
		return false;
	}

	UE_LOG(LogTemp, Warning,
	       TEXT("[UGZStartInteractionAbility::ServerValidateTargetData] TargetFromClient: %s, TargetFromServer: %s"),
	       *TargetFromClient->GetFullName(), *TargetFromServer->GetFullName());

	if (TargetFromClient != TargetFromServer || !AttemptInteraction_Internal(TargetFromServer))
	{
		Debug::Print(TEXT("[UGZStartInteractionAbility::ServerValidateTargetData] Interaction Failed"));
		return false;
	}
	return true;
}


void UGZStartInteractionAbility::OnFocusChanged(AActor* NewFocus, AActor* OldFocus)
{
	if (auto PawnFeature = GetPawnFeature())
	{
		if (NewFocus)
		{
			PawnFeature->OnFocusActor.Broadcast(NewFocus);
			UE_LOG(LogTemp, Log, TEXT("[UGZStartInteractionAbility::OnFocusChanged] New focus: %s"),
			       *NewFocus->GetActorNameOrLabel());
		}

		if (OldFocus)
		{
			PawnFeature->OnUnfocusActor.Broadcast(OldFocus);
			UE_LOG(LogTemp, Log, TEXT("[UGZStartInteractionAbility::OnFocusChanged] Old focus: %s"),
			       *OldFocus->GetActorNameOrLabel());
		}
	}
}
