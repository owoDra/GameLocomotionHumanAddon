// Copyright (C) 2024 owoDra

#pragma once

#include "Animation/AnimInstanceProxy.h"

#include "HumanAnimInstanceProxy.generated.h"

class UHumanAnimInstance;
class UHumanLinkedAnimInstance;

/**
 * This class is used to access some protected members of the FAnimInstanceProxy when using UHumanAnimInstance and UHumanLinkedAnimInstance
 */
USTRUCT()
struct GLHADDON_API FHumanAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	friend UHumanAnimInstance;
	friend UHumanLinkedAnimInstance;

private:
	typedef FAnimInstanceProxy Super;

public:
	FHumanAnimInstanceProxy() = default;

	explicit FHumanAnimInstanceProxy(UAnimInstance* AnimationInstance);

};
