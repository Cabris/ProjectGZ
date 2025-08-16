#include "Actor//GZWorldPickupActor.h"
#include "Components/SphereComponent.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Components/StaticMeshComponent.h"

AGZWorldPickupActor::AGZWorldPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(RootComponent);
	InteractCollision->InitSphereRadius(30.f);
	
	InteractCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractCollision->SetCollisionObjectType(ECC_WorldDynamic);

	// 對你的掃描頻道（例如 ECC_Visibility 或自訂 ECC_Interact）設為 Block
	InteractCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 換成你的 TraceChannel

	const UGZInventoryItemDefinition* ItemDefinition = ItemDefinitionClass.GetDefaultObject();
	if (ItemDefinition)
	{
		StaticMeshComponent->SetRelativeTransform(ItemDefinition->GetPickupMeshOffsetTransform());
		InteractCollision->SetRelativeTransform(ItemDefinition->GetPickupMeshOffsetTransform());
	}
}

FVector AGZWorldPickupActor::GetWorldPosition() const
{
	return  GetActorLocation();
}

FText AGZWorldPickupActor::GetInteravtionText() const
{
	return InteravtionText;
}

bool AGZWorldPickupActor::IsInteractable() const
{
	return bIsInteractable;
}

const TSubclassOf<UGZInteractAbility>& AGZWorldPickupActor::GetInteractAbilityClass()
{
	return InteractAbilityClass;
}

const TSubclassOf<UGZInventoryItemDefinition>& AGZWorldPickupActor::GetItemDefinitionClass() const
{
	return ItemDefinitionClass;
}

void AGZWorldPickupActor::DoInteract_Implementation()
{
	IGZInteractable::DoInteract_Implementation();
}

void AGZWorldPickupActor::OnBeginFocus_Implementation()
{
	IGZInteractable::OnBeginFocus_Implementation();
}

void AGZWorldPickupActor::OnEndFocus_Implementation()
{
	IGZInteractable::OnEndFocus_Implementation();
}
