#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
#define DEFAULT_GZ_ROTATE_RATE_YAW 400.0f
#define InteractionChannel ECC_GameTraceChannel1

namespace Debug
{
	static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, 7.f, Color, Msg);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}
	}

// 備用方案：巨集版本
#define DEBUG_PRINTF(Format, ...) \
do { \
if (GEngine) \
{ \
const FString FormattedMsg = FString::Printf(Format, ##__VA_ARGS__); \
GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::MakeRandomColor(), FormattedMsg); \
UE_LOG(LogTemp, Warning, TEXT("%s"), *FormattedMsg); \
} \
} while(0)
}
