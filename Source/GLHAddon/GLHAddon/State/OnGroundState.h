// Copyright (C) 2023 owoDra

#pragma once

#include "Type/MovementDirectionTypes.h"

#include "OnGroundState.generated.h"


UENUM(BlueprintType)
enum class EHipsDirection : uint8
{
	Forward,
	Backward,
	LeftForward,
	LeftBackward,
	RightForward,
	RightBackward,
};


USTRUCT(BlueprintType)
struct FVelocityBlendState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReinitializationRequired{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float ForwardAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float BackwardAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float LeftAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float RightAmount{ 0.0f };

};


USTRUCT(BlueprintType)
struct FRotationYawOffsetsState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float ForwardAngle{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float BackwardAngle{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float LeftAngle{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float RightAngle{ 0.0f };

};


USTRUCT(BlueprintType)
struct FOnGroundState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHipsDirection HipsDirection{ EHipsDirection::Forward };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -1, ClampMax = 1))
	float HipsDirectionLockAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPivotActivationRequested{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPivotActive{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementDirectionCache MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVelocityBlendState VelocityBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotationYawOffsetsState RotationYawOffsets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float SprintTime{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -1, ClampMax = 1))
	float SprintAccelerationAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float SprintBlockAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float WalkRunBlendAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float StrideBlendAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float StandingPlayRate{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float CrouchingPlayRate{ 1.0f };

};
