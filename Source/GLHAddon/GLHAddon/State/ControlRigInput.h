// Copyright (C) 2024 owoDra

#pragma once

#include "ControlRigInput.generated.h"

USTRUCT(BlueprintType)
struct FControlRigInput
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	bool bUseHandIkBones = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	bool bUseFootIkBones = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = 0, ClampMax = 1))
	float VelocityBlendForwardAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = 0, ClampMax = 1))
	float VelocityBlendBackwardAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float SpineYawAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat FootLeftIkRotation = FQuat(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector FootLeftIkLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = 0, ClampMax = 1))
	float FootLeftIkAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat FootRightIkRotation = FQuat(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector FootRightIkLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = 0, ClampMax = 1))
	float FootRightIkAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector2D MinMaxPelvisOffsetZ = FVector2D(ForceInit);
};
