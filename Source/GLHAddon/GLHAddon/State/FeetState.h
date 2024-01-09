// Copyright (C) 2024 owoDra

#pragma once

#include "State/SpringState.h"

#include "FeetState.generated.h"

USTRUCT(BlueprintType)
struct FFootState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = 0, ClampMax = 1))
	float IkAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = 0, ClampMax = 1))
	float LockAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector TargetLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat TargetRotation = FQuat(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector LockLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat LockRotation = FQuat(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector LockComponentRelativeLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat LockComponentRelativeRotation = FQuat(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector LockMovementBaseRelativeLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat LockMovementBaseRelativeRotation = FQuat(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector OffsetTargetLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat OffsetTargetRotation = FQuat(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FSpringVectorState OffsetSpringState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector OffsetLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat OffsetRotation = FQuat(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector IkLocation = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FQuat IkRotation = FQuat(ForceInit);
};

USTRUCT(BlueprintType)
struct FFeetState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = -1, ClampMax = 1))
	float FootPlantedAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "", Meta = (ClampMin = 0, ClampMax = 1))
	float FeetCrossingAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FFootState Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FFootState Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector2D MinMaxPelvisOffsetZ = FVector2D(ForceInit);
};
