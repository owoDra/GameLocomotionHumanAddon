// Copyright (C) 2024 owoDra

#include "AnimationModifier_CreateHumanLayeringCurves.h"

#include "LocomotionHumanNameStatics.h"

#include "AnimationBlueprintLibrary.h"
#include "Animation/AnimSequence.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimationModifier_CreateHumanLayeringCurves)

UAnimationModifier_CreateHumanLayeringCurves::UAnimationModifier_CreateHumanLayeringCurves()
{
	CurveNames = 
	{
		ULocomotionHumanNameStatics::LayerHeadCurveName(),
		ULocomotionHumanNameStatics::LayerHeadAdditiveCurveName(),
		ULocomotionHumanNameStatics::LayerArmLeftCurveName(),
		ULocomotionHumanNameStatics::LayerArmLeftAdditiveCurveName(),
		ULocomotionHumanNameStatics::LayerArmLeftLocalSpaceCurveName(),
		ULocomotionHumanNameStatics::LayerArmRightCurveName(),
		ULocomotionHumanNameStatics::LayerArmRightAdditiveCurveName(),
		ULocomotionHumanNameStatics::LayerArmRightLocalSpaceCurveName(),
		ULocomotionHumanNameStatics::LayerHandLeftCurveName(),
		ULocomotionHumanNameStatics::LayerHandRightCurveName(),
		ULocomotionHumanNameStatics::LayerSpineCurveName(),
		ULocomotionHumanNameStatics::LayerSpineAdditiveCurveName(),
		ULocomotionHumanNameStatics::LayerPelvisCurveName(),
		ULocomotionHumanNameStatics::LayerLegsCurveName(),

		ULocomotionHumanNameStatics::HandLeftIkCurveName(),
		ULocomotionHumanNameStatics::HandRightIkCurveName(),

		ULocomotionGeneralNameStatics::ViewBlockCurveName(),
		ULocomotionGeneralNameStatics::AllowAimingCurveName(),

		ULocomotionHumanNameStatics::HipsDirectionLockCurveName(),
	};

	SlotCurveNames =
	{
		ULocomotionHumanNameStatics::LayerHeadSlotCurveName(),
		ULocomotionHumanNameStatics::LayerArmLeftSlotCurveName(),
		ULocomotionHumanNameStatics::LayerArmRightSlotCurveName(),
		ULocomotionHumanNameStatics::LayerSpineSlotCurveName(),
		ULocomotionHumanNameStatics::LayerPelvisSlotCurveName(),
		ULocomotionHumanNameStatics::LayerLegsSlotCurveName(),
	};
}


void UAnimationModifier_CreateHumanLayeringCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	Super::OnApply_Implementation(Sequence);

	CreateCurves(Sequence, CurveNames, CurveValue);

	if (bAddSlotCurves)
	{
		CreateCurves(Sequence, SlotCurveNames, SlotCurveValue);
	}
}

void UAnimationModifier_CreateHumanLayeringCurves::CreateCurves(UAnimSequence* Sequence, const TArray<FName>& Names, const float Value) const
{
	for (const auto& CurveName : Names)
	{
		if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, CurveName, ERawCurveTrackTypes::RCT_Float))
		{
			if (!bOverrideExistingCurves)
			{
				continue;
			}

			UAnimationBlueprintLibrary::RemoveCurve(Sequence, CurveName);
		}

		UAnimationBlueprintLibrary::AddCurve(Sequence, CurveName);

		if (bAddKeyOnEachFrame)
		{
			for (auto i{ 0 }; i < Sequence->GetNumberOfSampledKeys(); i++)
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, CurveName, Sequence->GetTimeAtFrame(i), Value);
			}
		}
		else
		{
			UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, CurveName, Sequence->GetTimeAtFrame(0), Value);
		}
	}
}
