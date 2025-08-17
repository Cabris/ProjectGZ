#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GZAssetManager.generated.h"

UCLASS()
class PROJECTGZ_API UGZAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UGZAssetManager& Get();
	virtual void StartInitialLoading() override;
	static const FPrimaryAssetType DynamicMeshDataAsset;
	static const FPrimaryAssetType RoadMeshDataAsset;
	static const FPrimaryAssetType BuildingDataAsset;


	// ------------ Primary Asset：非同步載入（樣板） ------------

	/**
	 * 依 PrimaryAssetId 非同步載入 Primary Asset（可選擇載入指定 Bundle）
	 * @param Id        要載入的 Primary Asset Id（例如：Type=ItemDef, Name=HealthPotion）
	 * @param Bundles   要一併載入的 Asset Bundle（例如 "Default"、"UI"）
	 * @param OnLoaded  載入完成後的回呼（在遊戲執行緒呼叫）
	 * @param Priority  載入優先權（0=預設）
	 * @return          Streamable Handle；保存它可取消或延長生命週期
	 */
	template <typename AssetType>
	TSharedPtr<FStreamableHandle> LoadPrimaryAssetAsync(const FPrimaryAssetId& Id, const TArray<FName>& Bundles, TFunction<void(AssetType*)> OnLoaded,
	                                                    TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority)
	{
		if (!Id.IsValid())
		{
			if (OnLoaded) { OnLoaded(nullptr); }
			return nullptr;
		}

		// 使用 UAssetManager 內建流程（會同時載入指定的 Bundles）
		TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAsset(
			Id,
			Bundles,
			FStreamableDelegate::CreateLambda([Id, OnLoaded]()
			{
				// 回到主緒：取出已載入的 Primary Asset
				if (UGZAssetManager* AM = Cast<UGZAssetManager>(UAssetManager::GetIfInitialized()))
				{
					UObject* Obj = AM->GetPrimaryAssetObject(Id);
					AssetType* Typed = Cast<AssetType>(Obj);
					if (OnLoaded) { OnLoaded(Typed); }
				}
				else
				{
					if (OnLoaded) { OnLoaded(nullptr); }
				}
			}),
			Priority);

		return Handle;
	}

	/** 同步載入 Primary Asset（會阻塞；除初始化/編輯器工具外，不建議遊戲中使用） */
	template <typename AssetType>
	AssetType* LoadPrimaryAssetSync(const FPrimaryAssetId& Id, const TArray<FName>& Bundles = {})
	{
		if (!Id.IsValid()) { return nullptr; }
		// 先取得路徑，再同步載入
		const FSoftObjectPath Path = GetPrimaryAssetPath(Id);
		if (!Path.IsValid()) { return nullptr; }

		FStreamableManager& SM = GetStreamableManager();
		SM.RequestSyncLoad(Path, false);
		// 若指定 Bundles，需要另外同步處理（通常建議用 Async）
		return Cast<AssetType>(GetPrimaryAssetObject(Id));
	}

	// ------------ 軟資產：非同步載入（樣板） ------------

	template <typename AssetType>
	TSharedPtr<FStreamableHandle> LoadSoftAsync(const TSoftObjectPtr<AssetType>& Ref, UObject* WeakOwner, TFunction<void(AssetType*)> OnLoaded,
	                                            TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority)
	{
		if (Ref.IsNull())
		{
			if (OnLoaded) { OnLoaded(nullptr); }
			return nullptr;
		}
		if (!::IsValid(WeakOwner))
			return nullptr;

		FStreamableManager& SM = GetStreamableManager();
		TSharedPtr<FStreamableHandle> Handle = SM.RequestAsyncLoad(
			Ref.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(WeakOwner,
              [Ref, OnLoaded]()
              {
                  AssetType* Asset = Ref.Get();
                  if (OnLoaded) { OnLoaded(Asset); }
              }),
			Priority);

		return Handle;
	}

	/** 同步載入軟資產（阻塞；謹慎使用） */
	template <typename AssetType>
	AssetType* LoadSoftSync(const TSoftObjectPtr<AssetType>& Ref)
	{
		return Ref.IsNull() ? nullptr : Ref.LoadSynchronous();
	}

	// ------------ 軟類別（藍圖類）載入（樣板） ------------

	template <typename ClassType /*:UObject*/>
	TSharedPtr<FStreamableHandle> LoadSoftClassAsync(const TSoftClassPtr<ClassType>& ClassRef, TFunction<void(UClass*)> OnLoaded,
	                                                 TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority)
	{
		if (ClassRef.IsNull())
		{
			if (OnLoaded) { OnLoaded(nullptr); }
			return nullptr;
		}

		FStreamableManager& SM = GetStreamableManager();
		TSharedPtr<FStreamableHandle> Handle = SM.RequestAsyncLoad(
			ClassRef.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([ClassRef, OnLoaded]()
			{
				UClass* Cls = ClassRef.Get();
				if (OnLoaded) { OnLoaded(Cls); }
			}),
			Priority);

		return Handle;
	}

	// ------------ 卸載/取消 ------------

	/** 依 PrimaryAssetId 卸載（包含它的 bundles） */
	void UnloadPrimary(const FPrimaryAssetId& Id)
	{
		if (Id.IsValid())
		{
			UnloadPrimaryAsset(Id);
		}
	}

	/** 釋放/取消某個 Streamable Handle（若仍在載入中可取消） */
	static void ReleaseHandle(const TSharedPtr<FStreamableHandle>& Handle, bool bCancelIfLoading = false)
	{
		if (!Handle.IsValid()) return;
		if (bCancelIfLoading) { Handle->CancelHandle(); }
		Handle->ReleaseHandle();
	}
};
