// Copyright (C) 2024 owoDra

#pragma once

#include "InAirState.generated.h"


USTRUCT(BlueprintType)
struct FInAirState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ForceUnits = "cm/s"))
	float VerticalVelocity{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumped{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float JumpPlayRate{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float GroundPredictionAmount{ 1.0f };

};
