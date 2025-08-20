#include "Actor//GZWorldPickupActor.h"
#include "Components/SphereComponent.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Components/StaticMeshComponent.h"
#include "Game/GZAssetManager.h"
#include "ProjectGZ/ProjectGZ.h"

AGZWorldPickupActor::AGZWorldPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(GetRootComponent());
	InteractCollision->InitSphereRadius(30.f);

	InteractCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractCollision->SetCollisionObjectType(ECC_WorldDynamic);

	InteractCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractCollision->SetCollisionResponseToChannel(InteractionChannel, ECR_Overlap);
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

void AGZWorldPickupActor::BeginPlay()
{
	Super::BeginPlay();
	SetupComponents();
}

FVector AGZWorldPickupActor::GetWorldPosition() const
{
	return GetActorLocation();
}

FText AGZWorldPickupActor::GetInteractionText() const
{
	return InteractionText;
}

bool AGZWorldPickupActor::IsInteractable() const
{
	return bIsInteractable;
}

const FGameplayTag& AGZWorldPickupActor::GetInteractAbilityTriggerTag()
{
	return InteractAbilityTriggerTag;
}

#if WITH_EDITOR
void AGZWorldPickupActor::PostEditChangeProperty(struct FPropertyChangedEvent& E)
{
	Super::PostEditChangeProperty(E);
	if (E.Property && E.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AGZWorldPickupActor, DefaultMesh)||
		 E.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AGZWorldPickupActor, ItemDefinitionClass))
	{
		SetupComponents();
	}
}
#endif

void AGZWorldPickupActor::PostLoad()
{
	Super::PostLoad();
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		SetupComponents();
	}
}

const TSubclassOf<UGZInventoryItemDefinition>& AGZWorldPickupActor::GetItemDefinitionClass() const
{
	return ItemDefinitionClass;
}

void AGZWorldPickupActor::ConsumeItemQuantity(int32)
{
}

void AGZWorldPickupActor::DoInteract_Implementation()
{
	IGZInteractable::DoInteract_Implementation();
	GEngine->AddOnScreenDebugMessage(
		0, // Key
		3.0f, // Display time（0 表示直到下一次呼叫取代）
		FColor::Green, // 顏色
		FString("DoInteract_Implementation")
	);
}

void AGZWorldPickupActor::OnBeginFocus_Implementation()
{
	IGZInteractable::OnBeginFocus_Implementation();
	GEngine->AddOnScreenDebugMessage(
		0, // Key
		3.0f, // Display time（0 表示直到下一次呼叫取代）
		FColor::Green, // 顏色
		FString("OnBeginFocus_Implementation")
	);
}

void AGZWorldPickupActor::OnEndFocus_Implementation()
{
	IGZInteractable::OnEndFocus_Implementation();
	GEngine->AddOnScreenDebugMessage(
		0, // Key
		3.0f, // Display time（0 表示直到下一次呼叫取代）
		FColor::Green, // 顏色
		FString("OnEndFocus_Implementation")
	);
}
