// Copyright (C) 2023 owoDra

#include "HumanLocomotionFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HumanLocomotionFunctionLibrary)


float UHumanLocomotionFunctionLibrary::SpringDampFloat(float Current, float Target, UPARAM(ref)FSpringFloatState& SpringState, float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount)
{
	return SpringDamp(Current, Target, SpringState, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}

FVector UHumanLocomotionFunctionLibrary::SpringDampVector(const FVector& Current, const FVector& Target, UPARAM(ref)FSpringVectorState& SpringState, float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount)
{
	return SpringDamp(Current, Target, SpringState, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}

EMovementDirection UHumanLocomotionFunctionLibrary::CalculateMovementDirection(float Angle, float ForwardHalfAngle, float AngleThreshold)
{
	if (Angle >= -ForwardHalfAngle - AngleThreshold && Angle <= ForwardHalfAngle + AngleThreshold)
	{
		return EMovementDirection::Forward;
	}

	if (Angle >= ForwardHalfAngle - AngleThreshold && Angle <= 180.0f - ForwardHalfAngle + AngleThreshold)
	{
		return EMovementDirection::Right;
	}

	if (Angle <= -(ForwardHalfAngle - AngleThreshold) && Angle >= -(180.0f - ForwardHalfAngle + AngleThreshold))
	{
		return EMovementDirection::Left;
	}

	return EMovementDirection::Backward;
}
