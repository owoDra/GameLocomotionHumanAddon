// Copyright (C) 2023 owoDra

#pragma once

#include "LeanState.generated.h"

USTRUCT(BlueprintType)
struct FLeanState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = -1, ClampMax = 1))
	float RightAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = -1, ClampMax = 1))
	float ForwardAmount = 0.0f;
};
