#pragma once
#include "NativeGameplayTags.h"
#include "Game/GZGameplayTagList.h" // 清單巨集

namespace GZGameplayTags
{
	// 在 .h 只展開成 extern 宣告
#define DECLARE_TAG(Name, StringPath, CommentText) UE_DECLARE_GAMEPLAY_TAG_EXTERN(Name);
	GZ_FOR_EACH_GAMEPLAY_TAG(DECLARE_TAG)
#undef DECLARE_TAG
}
/*
struct FGZGameplayTags
{
public:
	static FGZGameplayTags& Get() { return Instance; }
	static void Init();
	//FGameplayTag Attributes_Vigor_Health;

private:
	static FGZGameplayTags Instance;
};
*/