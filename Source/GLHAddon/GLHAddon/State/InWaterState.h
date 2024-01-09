// Copyright (C) 2024 owoDra

#pragma once

#include "InWaterState.generated.h"


USTRUCT(BlueprintType)
struct FInWaterState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUnderwater{ false };

};
