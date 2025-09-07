#include "Data/GZGameplayCueCatalog.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayCueSet.h"
#include "Engine/AssetManager.h"

void UGZGameplayCueCatalog::RegisterCatalogToCueSet(const UGZGameplayCueCatalog* Catalog)
{
	if (!Catalog) return;

	//Create data struct for cue set
	TArray<FGameplayCueReferencePair> Pairs;
	Pairs.Reserve(Catalog->Entries.Num());
	for (const FGZCueEntry& E : Catalog->Entries)
	{
		if (!E.CueTag.IsValid() || !E.NotifyClass.ToSoftObjectPath().IsValid()) continue;
		Pairs.Emplace(E.CueTag, E.NotifyClass.ToSoftObjectPath());//use soft path to avoid hard reference
	}
	//Add GC from Catalog
	if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
	{
		if (UGameplayCueSet* Set = GCM->GetRuntimeCueSet())
		{
			Set->AddCues(Pairs);
		}
	}

	//async load GC from catalog
	TArray<FSoftObjectPath> ToLoad;
	for (const FGZCueEntry& E : Catalog->Entries)
	{
		if (E.NotifyClass.IsNull()) continue;
		ToLoad.AddUnique(E.NotifyClass.ToSoftObjectPath());
	}
	UAssetManager::GetStreamableManager().RequestAsyncLoad(ToLoad, FStreamableDelegate());
}
