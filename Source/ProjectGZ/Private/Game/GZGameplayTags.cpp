#include "Game/GZGameplayTags.h"
#include "Game/GZGameplayTagList.h" // 清單巨集

namespace GZGameplayTags
{
	// 關鍵：在 .cpp 原始檔文本中展開成 DEFINE，
	// UE_DEFINE_GAMEPLAY_TAG_COMMENT 現在「真的」在 .cpp 內展開，因此不會觸發 static_assert。
#define DEFINE_TAG(Name, StringPath, CommentText) UE_DEFINE_GAMEPLAY_TAG_COMMENT(Name, StringPath, CommentText);
	GZ_FOR_EACH_GAMEPLAY_TAG(DEFINE_TAG)
#undef DEFINE_TAG
}


FGZGameplayTags FGZGameplayTags::Instance;

void FGZGameplayTags::Init()
{
	Instance.Attributes_Vigor_Health = UGameplayTagsManager::Get().AddNativeGameplayTag("Attributes.Vigor.Health", "");
}
