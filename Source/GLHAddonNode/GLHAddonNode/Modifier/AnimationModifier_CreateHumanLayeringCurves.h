// Copyright (C) 2023 owoDra

#pragma once

#include "AnimationModifier.h"

#include "AnimationModifier_CreateHumanLayeringCurves.generated.h"


UCLASS(DisplayName = "AM Create Layering Curves For Human")
class GLHADDONNODE_API UAnimationModifier_CreateHumanLayeringCurves : public UAnimationModifier
{
	GENERATED_BODY()
public:
	UAnimationModifier_CreateHumanLayeringCurves();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bOverrideExistingCurves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bAddKeyOnEachFrame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float CurveValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<FName> CurveNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bAddSlotCurves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float SlotCurveValue{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<FName> SlotCurveNames;

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;

private:
	void CreateCurves(UAnimSequence* Sequence, const TArray<FName>& Names, float Value) const;

};
