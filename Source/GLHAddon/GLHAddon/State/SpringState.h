// Copyright (C) 2023 owoDra

#pragma once

#include "SpringState.generated.h"

USTRUCT(BlueprintType)
struct FSpringFloatState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	float Velocity = ForceInit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	float PreviousTarget = ForceInit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	bool bStateValid = false;

public:
	void Reset()
	{
		Velocity = 0.f;
		PreviousTarget = 0.f;
		bStateValid = false;
	}
};

USTRUCT(BlueprintType)
struct FSpringVectorState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector Velocity = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	FVector PreviousTarget = FVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
	bool bStateValid = false;

public:
	void Reset()
	{
		Velocity = FVector::ZeroVector;
		PreviousTarget = FVector::ZeroVector;
		bStateValid = false;
	}
};
