// Copyright (C) 2024 owoDra

#pragma once

#include "LookState.generated.h"


USTRUCT(BlueprintType)
struct FLookState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReinitializationRequired{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float WorldYawAngle{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float YawAngle{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -90, ClampMax = 90, ForceUnits = "deg"))
	float PitchAngle{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float YawForwardAmount{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 0.5))
	float YawLeftAmount{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.5, ClampMax = 1))
	float YawRightAmount{ 0.5f };

};
