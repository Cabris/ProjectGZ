#include "Actor//GZWorldPickupActor.h"
#include "Components/SphereComponent.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Components/StaticMeshComponent.h"
#include "Game/GZAssetManager.h"
#include "ProjectGZ/ProjectGZ.h"

AGZWorldPickupActor::AGZWorldPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	UE_LOG(LogTemp, Log, TEXT("AGZWorldPickupActor::Constructor(): %s"), *GetName());
}

void AGZWorldPickupActor::SetupComponents()
{
	if (IsValid(DefaultMesh))
		StaticMeshComponent->SetStaticMesh(DefaultMesh);

	if (!IsValid(ItemDefinitionClass))
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinitionClass Null"));
		return;
	}
	const UGZInventoryItemDefinition* ItemDefinition = ItemDefinitionClass.GetDefaultObject();
	if (!ItemDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinition Null"));
		return;
	}

	TSoftObjectPtr<UStaticMesh> MeshSoftPtr = ItemDefinition->GetPickupMeshSoftPtr();
	if (MeshSoftPtr.Get())
	{
		StaticMeshComponent->SetStaticMesh(MeshSoftPtr.Get());
	}
	else if (!MeshSoftPtr.IsNull())
	{
		UGZAssetManager::Get().LoadSoftAsync<UStaticMesh>(MeshSoftPtr, this,
		                                                  [this](UStaticMesh* Mesh)
		                                                  {
			                                                  StaticMeshComponent->SetStaticMesh(Mesh);
		                                                  });
	}

	StaticMeshComponent->SetRelativeTransform(ItemDefinition->GetPickupMeshOffsetTransform());
	InteractCollision->SetRelativeTransform(ItemDefinition->GetPickupMeshOffsetTransform());
}

#if WITH_EDITOR
void AGZWorldPickupActor::PostEditChangeProperty(struct FPropertyChangedEvent& E)
{
	Super::PostEditChangeProperty(E);
	if (E.Property && E.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, DefaultMesh) ||
		E.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, ItemDefinitionClass))
	{
		SetupComponents();
	}
}
#endif

const TSubclassOf<UGZInventoryItemDefinition>& AGZWorldPickupActor::GetItemDefinitionClass() const
{
	return ItemDefinitionClass;
}

void AGZWorldPickupActor::ConsumeItemQuantity(int32 Quantity)
{
	UE_LOG(LogTemp, Log, TEXT("AGZWorldPickupActor::ConsumeItemQuantity: %d"), Quantity);
}

void AGZWorldPickupActor::OnCollected_Implementation()
{
	Debug::Print(TEXT("AGZWorldPickupActor::OnCollected_Implementation"));
}
