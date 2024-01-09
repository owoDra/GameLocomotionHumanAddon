// Copyright (C) 2024 owoDra

#pragma once

#include "AnimationModifier.h"

#include "AnimationModifier_CreateHumanCurves.generated.h"


/**
 * Keyframe data for the curve to be added
 */
USTRUCT(BlueprintType)
struct GLHADDONNODE_API FAnimationCurveKey
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	int32 Frame{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value{ 0.0f };

};


/**
 * Curve data to be added
 */
USTRUCT(BlueprintType)
struct GLHADDONNODE_API FAnimationCurve
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAddKeyOnEachFrame{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAnimationCurveKey> Keys{ {0, 0.0f} };

};


/**
 * Curve data to be added
 */
UCLASS(DisplayName = "AM Create Curves For Human")
class GLHADDONNODE_API UAnimationModifier_CreateHumanCurves : public UAnimationModifier
{
	GENERATED_BODY()
public:
	UAnimationModifier_CreateHumanCurves();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bOverrideExistingCurves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<FAnimationCurve> Curves;

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;

};
