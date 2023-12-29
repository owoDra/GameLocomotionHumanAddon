// Copyright (C) 2023 owoDra

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "State/SpringState.h"
#include "Type/MovementDirectionTypes.h"

#include "HumanLocomotionFunctionLibrary.generated.h"


UCLASS()
class GLHADDON_API UHumanLocomotionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <typename ValueType, typename StateType>
	static ValueType SpringDamp(const ValueType& Current, const ValueType& Target, StateType& SpringState, float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f)
	{
		if (DeltaTime <= UE_SMALL_NUMBER)
		{
			return Current;
		}

		if (!SpringState.bStateValid)
		{
			SpringState.Velocity = ValueType{ 0.0f };
			SpringState.PreviousTarget = Target;
			SpringState.bStateValid = true;

			return Target;
		}

		const auto ClampedTargetVelocityAmount
		{
			(TargetVelocityAmount <= 0.0f) ? 0.0f : (TargetVelocityAmount >= 1.0f) ? 1.0f : TargetVelocityAmount
		};

		ValueType Result{ Current };
		FMath::SpringDamper(Result, SpringState.Velocity, Target,
			(Target - SpringState.PreviousTarget) * (ClampedTargetVelocityAmount / DeltaTime),
			DeltaTime, Frequency, DampingRatio);

		SpringState.PreviousTarget = Target;

		return Result;
	}

	UFUNCTION(BlueprintCallable, Category = "Movement", Meta = (ReturnDisplayName = "Value"))
	static float SpringDampFloat(float Current, float Target, UPARAM(ref) FSpringFloatState& SpringState, float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Movement", Meta = (AutoCreateRefTerm = "Current, Target", ReturnDisplayName = "Vector"))
	static FVector SpringDampVector(const FVector& Current, const FVector& Target, UPARAM(ref) FSpringVectorState& SpringState, float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Movement|Input", Meta = (ReturnDisplayName = "Direction"))
	static EMovementDirection CalculateMovementDirection(float Angle, float ForwardHalfAngle, float AngleThreshold);

};
