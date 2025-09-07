#include "Game/GZGameplayCueManager.h"
#include "AbilitySystemGlobals.h"

UGZGameplayCueManager* UGZGameplayCueManager::Get()
{
	auto GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager();
	if (GCM == nullptr) return nullptr;
	return Cast<UGZGameplayCueManager>(GCM);
}

void UGZGameplayCueManager::StartInitialLoading()
{
	
}
