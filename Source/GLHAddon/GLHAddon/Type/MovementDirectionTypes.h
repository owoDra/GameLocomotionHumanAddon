// Copyright (C) 2023 owoDra

#pragma once

#include "MovementDirectionTypes.generated.h"


UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right
};


USTRUCT(BlueprintType)
struct FMovementDirectionCache
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EMovementDirection MovementDirection{ EMovementDirection::Forward };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bForward{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bBackward{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bLeft{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bRight{ false };

public:
	constexpr FMovementDirectionCache() = default;

	constexpr FMovementDirectionCache(const EMovementDirection NewMovementDirection)
	{
		MovementDirection = NewMovementDirection;

		bForward = MovementDirection == EMovementDirection::Forward;
		bBackward = MovementDirection == EMovementDirection::Backward;
		bLeft = MovementDirection == EMovementDirection::Left;
		bRight = MovementDirection == EMovementDirection::Right;
	}

	constexpr bool IsForward() const
	{
		return bForward;
	}

	constexpr bool IsBackward() const
	{
		return bBackward;
	}

	constexpr bool IsLeft() const
	{
		return bLeft;
	}

	constexpr bool IsRight() const
	{
		return bRight;
	}

	constexpr operator EMovementDirection() const
	{
		return MovementDirection;
	}

};
