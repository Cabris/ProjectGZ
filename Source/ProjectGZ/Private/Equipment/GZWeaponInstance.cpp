#include "Equipment/GZWeaponInstance.h"

FFireResult UGZWeaponInstance::CalculateFireResult(const FFireParams& Params) const
{
	FFireResult FireResult = {false}; //bIsValid=false
	if (!ensure(Params.Direction.IsNormalized()))
		return FireResult;

	const FVector Start = Params.Origin;
	const FVector End = Start + Params.Direction * Params.MaxRange;

	FCollisionQueryParams QueryParams;
	BuildCollisionParams(Params.Filter, QueryParams);

	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Params.Filter.TraceChannel, QueryParams);

	if (bHit)
	{
		FireResult.bIsValid = true;
		FireResult.Hit = Hit;
		FireResult.bIsHit = true;
		FireResult.EndLocation = Hit.ImpactPoint;
	}
	else
	{
		FireResult.bIsValid = true;
		FireResult.EndLocation = End;
	}
#ifdef  WITH_EDITOR
	if (Params.bDrawDebug)
	{
		DrawDebugLine(GetWorld(), Start, FireResult.EndLocation, FColor::MakeRandomColor(), false, 0.5f, 0, 1.0f);
	}
#endif

	return FireResult;
}

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
