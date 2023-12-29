// Copyright (C) 2023 owoDra

#include "AnimationModifier_CreateHumanCurves.h"

#include "LocomotionHumanNameStatics.h"

#include "LocomotionGeneralNameStatics.h"

#include "AnimationBlueprintLibrary.h"
#include "Animation/AnimSequence.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimationModifier_CreateHumanCurves)


UAnimationModifier_CreateHumanCurves::UAnimationModifier_CreateHumanCurves()
{
	Curves = 
	{
		{ ULocomotionHumanNameStatics::PoseGaitCurveName() },
		{ ULocomotionHumanNameStatics::PoseMovingCurveName() },
		{ ULocomotionHumanNameStatics::PoseStandingCurveName() },
		{ ULocomotionHumanNameStatics::PoseCrouchingCurveName() },
		{ ULocomotionHumanNameStatics::PoseInAirCurveName() },
		{ ULocomotionHumanNameStatics::PoseGroundedCurveName() },

		{ ULocomotionHumanNameStatics::FootLeftIkCurveName() },
		{ ULocomotionHumanNameStatics::FootLeftLockCurveName() },
		{ ULocomotionHumanNameStatics::FootRightIkCurveName() },
		{ ULocomotionHumanNameStatics::FootRightLockCurveName() },
		{ ULocomotionHumanNameStatics::FootPlantedCurveName() },
		{ ULocomotionHumanNameStatics::FeetCrossingCurveName() },

		{ ULocomotionHumanNameStatics::SprintBlockCurveName() },
		{ ULocomotionGeneralNameStatics::AllowTransitionsCurveName() },
		{ ULocomotionGeneralNameStatics::GroundPredictionBlockCurveName() },
		{ ULocomotionGeneralNameStatics::FootstepSoundBlockCurveName() }
	};
}


void UAnimationModifier_CreateHumanCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	Super::OnApply_Implementation(Sequence);

	for (const auto& Curve : Curves)
	{
		if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, Curve.Name, ERawCurveTrackTypes::RCT_Float))
		{
			if (!bOverrideExistingCurves)
			{
				continue;
			}

			UAnimationBlueprintLibrary::RemoveCurve(Sequence, Curve.Name);
		}

		UAnimationBlueprintLibrary::AddCurve(Sequence, Curve.Name);

		if (Curve.bAddKeyOnEachFrame)
		{
			for (auto i{ 0 }; i < Sequence->GetNumberOfSampledKeys(); i++)
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, Curve.Name, Sequence->GetTimeAtFrame(i), 0);
			}
		}
		else
		{
			for (const auto& CurveKey : Curve.Keys)
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, Curve.Name, Sequence->GetTimeAtFrame(CurveKey.Frame), CurveKey.Value);
			}
		}
	}
}
