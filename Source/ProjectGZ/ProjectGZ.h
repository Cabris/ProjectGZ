// Copyright Epic Games, Inc. All Rights Reserved.

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
			GEngine->AddOnScreenDebugMessage(InKey,7.f,Color,Msg);

			UE_LOG(LogTemp,Warning,TEXT("%s"),*Msg);
		}
	}
}