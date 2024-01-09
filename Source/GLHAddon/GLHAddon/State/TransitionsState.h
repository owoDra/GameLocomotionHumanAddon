// Copyright (C) 2024 owoDra

#pragma once

#include "TransitionsState.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct FTransitionsState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	bool bTransitionsAllowed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	int32 DynamicTransitionsFrameDelay = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	TObjectPtr<UAnimSequenceBase> QueuedDynamicTransitionAnimation = nullptr;
};
