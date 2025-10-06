#include "Equipment/GZWeaponInstance.h"

#include "Equipment/GZEquipmentDefinition.h"
#include "Inventory/GZInventoryItemInstance.h"

void UGZWeaponInstance::BuildCollisionParams(const FAttackFilter& Filter, FCollisionQueryParams& OutQuery)
{
	OutQuery = FCollisionQueryParams(SCENE_QUERY_STAT(GZ_FireTrace), Filter.bTraceComplex);

	// 1) 忽略 Instigator 自己
	if (const AActor* Inst = Filter.Instigator.Get())
	{
		OutQuery.AddIgnoredActor(Inst);

		// 2) 忽略 Instigator 的「附掛演員」與 ChildActors
		if (Filter.bIgnoreInstigatorAndAttachments)
		{
			TArray<AActor*> Attached;
			const_cast<AActor*>(Inst)->GetAttachedActors(Attached, /*bResetArray*/ true);
			for (AActor* A : Attached)
			{
				OutQuery.AddIgnoredActor(A);
			}

			// ChildActorComponent 生成的子演員
			TArray<AActor*> Childs;
			const_cast<AActor*>(Inst)->GetAllChildActors(Childs, /*bIncludeDescendants*/ true);
			for (AActor* A : Childs)
			{
				OutQuery.AddIgnoredActor(A);
			}
		}
	}

	// 3) 其它需要忽略的演員
	for (const TWeakObjectPtr<const AActor>& WeakA : Filter.ExtraIgnoredActors)
	{
		if (const AActor* A = WeakA.Get())
		{
			OutQuery.AddIgnoredActor(A);
		}
	}
}

FName UGZWeaponInstance::GetWeaponMuzzleSocketName() const
{
	auto EquipmentDefClass = GetItemInstance()->GetItemDefinition()->GetEquipmentDef();
	auto& ActorsToSpawn = EquipmentDefClass.GetDefaultObject()->ActorsToSpawn;
	for (auto& SpawnData: ActorsToSpawn)
	{
		if (!SpawnData.MuzzleSocket.IsNone())
			return SpawnData.MuzzleSocket;
	}
	return FName();
}
