// Copyright (C) 2023 owoDra

#include "HumanAnimInstance.h"

#include "LocomotionHumanNameStatics.h"
#include "HumanLocomotionFunctionLibrary.h"
#include "HumanAnimInstanceProxy.h"

#include "LocomotionGeneralNameStatics.h"
#include "LocomotionFunctionLibrary.h"
#include "LocomotionComponent.h"
#include "LocomotionCharacter.h"
#include "GameplayTag/GCLATags_Status.h"
#include "GCLAddonStatGroup.h"

#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HumanAnimInstance)


UHumanAnimInstance::UHumanAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FAnimInstanceProxy* UHumanAnimInstance::CreateAnimInstanceProxy()
{
	return new FHumanAnimInstanceProxy(this);
}


void UHumanAnimInstance::UpdateAnimationOnGameThread(float DeltaTime)
{
	if (!IsValid(Character) || !IsValid(CharacterMovement))
	{
		return;
	}

	if (GetSkelMeshComponent()->IsUsingAbsoluteRotation())
	{
		const auto& ActorTransform{ Character->GetActorTransform() };

		// Manual synchronization of mesh rotation with character rotation

		GetSkelMeshComponent()->MoveComponent(FVector::ZeroVector, ActorTransform.GetRotation() * Character->GetBaseRotationOffset(), false);

		// Re-cache proxy transforms to match the modified mesh transform.

		const auto& Proxy{ GetProxyOnGameThread<FAnimInstanceProxy>() };

		const_cast<FTransform&>(Proxy.GetComponentTransform()) = GetSkelMeshComponent()->GetComponentTransform();
		const_cast<FTransform&>(Proxy.GetComponentRelativeTransform()) = GetSkelMeshComponent()->GetRelativeTransform();
		const_cast<FTransform&>(Proxy.GetActorTransform()) = ActorTransform;
	}

	UpdateCharacterStatesOnGameThread();
	UpdateMovementBaseOnGameThread();
	UpdateViewOnGameThread();
	UpdateLocomotionOnGameThread();
	UpdateGroundedOnGameThread();
	UpdateInAirOnGameThread();
	UpdateInWaterOnGameThread();

	UpdateFeetOnGameThread();
}

void UHumanAnimInstance::UpdateAnimationOnThreadSafe(float DeltaTime)
{
	if (!IsValid(Character) || !IsValid(CharacterMovement))
	{
		return;
	}

	UpdateLayering();
	UpdatePose();

	UpdateView(DeltaTime);
	UpdateSpineRotation(DeltaTime);
	UpdateGrounded(DeltaTime);
	UpdateInAir(DeltaTime);
	UpdateInWater(DeltaTime);

	UpdateFeet(DeltaTime);

	UpdateTransitions();
	UpdateRotateInPlace(DeltaTime);
}

void UHumanAnimInstance::OnPostEvaluateAnimation()
{
	if (!IsValid(Character) || !IsValid(CharacterMovement))
	{
		return;
	}

	PlayQueuedDynamicTransitionAnimation();

	bPendingUpdate = false;
}


#pragma region Layering State

void UHumanAnimInstance::UpdateLayering()
{
	const auto& Curves{ GetProxyOnAnyThread<FHumanAnimInstanceProxy>().GetAnimationCurves(EAnimCurveType::AttributeCurve) };

	// Lambda function to get the value of AnimCurve

	static const auto GetCurveValue
	{
		[](const TMap<FName, float>& Curves, const FName& CurveName) -> float
		{
			const float* Value{Curves.Find(CurveName)};

			return (Value != nullptr) ? *Value : 0.0f;
		}
	};

	LayeringState.HeadBlendAmount				= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerHeadCurveName());
	LayeringState.HeadAdditiveBlendAmount		= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerHeadAdditiveCurveName());
	LayeringState.HeadSlotBlendAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerHeadSlotCurveName());

	LayeringState.ArmLeftBlendAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerArmLeftCurveName());
	LayeringState.ArmLeftAdditiveBlendAmount	= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerArmLeftAdditiveCurveName());
	LayeringState.ArmLeftSlotBlendAmount		= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerArmLeftSlotCurveName());
	LayeringState.ArmLeftLocalSpaceBlendAmount	= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerArmLeftLocalSpaceCurveName());
	LayeringState.ArmLeftMeshSpaceBlendAmount	= !FAnimWeight::IsFullWeight(LayeringState.ArmLeftLocalSpaceBlendAmount);

	LayeringState.ArmRightBlendAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerArmRightCurveName());
	LayeringState.ArmRightAdditiveBlendAmount	= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerArmRightAdditiveCurveName());
	LayeringState.ArmRightSlotBlendAmount		= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerArmRightSlotCurveName());
	LayeringState.ArmRightLocalSpaceBlendAmount = GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerArmRightLocalSpaceCurveName());
	LayeringState.ArmRightMeshSpaceBlendAmount	= !FAnimWeight::IsFullWeight(LayeringState.ArmRightLocalSpaceBlendAmount);

	LayeringState.HandLeftBlendAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerHandLeftCurveName());
	LayeringState.HandRightBlendAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerHandRightCurveName());

	LayeringState.SpineBlendAmount				= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerSpineCurveName());
	LayeringState.SpineAdditiveBlendAmount		= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerSpineAdditiveCurveName());
	LayeringState.SpineSlotBlendAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerSpineSlotCurveName());

	LayeringState.PelvisBlendAmount				= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerPelvisCurveName());
	LayeringState.PelvisSlotBlendAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerPelvisSlotCurveName());

	LayeringState.LegsBlendAmount				= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerLegsCurveName());
	LayeringState.LegsSlotBlendAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::LayerLegsSlotCurveName());
}

#pragma endregion


#pragma region Pose State

void UHumanAnimInstance::UpdatePose()
{
	const auto& Curves{ GetProxyOnAnyThread<FHumanAnimInstanceProxy>().GetAnimationCurves(EAnimCurveType::AttributeCurve) };

	// Lambda function to get the value of AnimCurve

	static const auto GetCurveValue
	{
		[](const TMap<FName, float>& Curves, const FName& CurveName) -> float
		{
			const float* Value{Curves.Find(CurveName)};

			return (Value != nullptr) ? *Value : 0.0f;
		}
	};

	PoseState.GroundedAmount		= GetCurveValue(Curves, ULocomotionHumanNameStatics::PoseGroundedCurveName());
	PoseState.InAirAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::PoseInAirCurveName());

	PoseState.StandingAmount		= GetCurveValue(Curves, ULocomotionHumanNameStatics::PoseStandingCurveName());
	PoseState.CrouchingAmount		= GetCurveValue(Curves, ULocomotionHumanNameStatics::PoseCrouchingCurveName());

	PoseState.MovingAmount			= GetCurveValue(Curves, ULocomotionHumanNameStatics::PoseMovingCurveName());

	PoseState.GaitAmount			= FMath::Clamp(GetCurveValue(Curves, ULocomotionHumanNameStatics::PoseGaitCurveName()), 0.0f, 3.0f);
	PoseState.GaitWalkingAmount		= ULocomotionFunctionLibrary::Clamp01(PoseState.GaitAmount);
	PoseState.GaitRunningAmount		= ULocomotionFunctionLibrary::Clamp01(PoseState.GaitAmount - 1.0f);
	PoseState.GaitSprintingAmount	= ULocomotionFunctionLibrary::Clamp01(PoseState.GaitAmount - 2.0f);

	// Unweight" the Walk Pose curve using the value of the Ground Pose curve
	// It instantly retrieves the full yield value from the beginning of the transition to the ground state

	PoseState.UnweightedGaitAmount	= (PoseState.GroundedAmount > 0.0f) ? (PoseState.GaitAmount / PoseState.GroundedAmount) : PoseState.GaitAmount;

	PoseState.UnweightedGaitWalkingAmount	= ULocomotionFunctionLibrary::Clamp01(PoseState.UnweightedGaitAmount);
	PoseState.UnweightedGaitRunningAmount	= ULocomotionFunctionLibrary::Clamp01(PoseState.UnweightedGaitAmount - 1.0f);
	PoseState.UnweightedGaitSprintingAmount = ULocomotionFunctionLibrary::Clamp01(PoseState.UnweightedGaitAmount - 2.0f);
}

#pragma endregion


#pragma region Spine Rotation State

void UHumanAnimInstance::UpdateSpineRotation(float DeltaTime)
{
	if (SpineRotationState.bSpineRotationAllowed != IsSpineRotationAllowed())
	{
		SpineRotationState.bSpineRotationAllowed = !SpineRotationState.bSpineRotationAllowed;
		SpineRotationState.StartYawAngle = SpineRotationState.CurrentYawAngle;
	}

	if (SpineRotationState.bSpineRotationAllowed)
	{
		static constexpr auto InterpolationSpeed{ 20.0f };

		SpineRotationState.SpineAmount = bPendingUpdate ? 1.0f : ULocomotionFunctionLibrary::ExponentialDecay(SpineRotationState.SpineAmount, 1.0f, DeltaTime, InterpolationSpeed);

		SpineRotationState.TargetYawAngle = ViewState.YawAngle;
	}
	else
	{
		static constexpr auto InterpolationSpeed{ 10.0f };

		SpineRotationState.SpineAmount = bPendingUpdate ? 0.0f : ULocomotionFunctionLibrary::ExponentialDecay(SpineRotationState.SpineAmount, 0.0f, DeltaTime, InterpolationSpeed);
	}

	SpineRotationState.CurrentYawAngle = ULocomotionFunctionLibrary::LerpAngle(SpineRotationState.StartYawAngle, SpineRotationState.TargetYawAngle, SpineRotationState.SpineAmount);

	SpineRotationState.YawAngle = SpineRotationState.CurrentYawAngle;
	SpineRotationState.YawAngle *= (ViewState.ViewAmount * ViewState.AimingAmount);
}

bool UHumanAnimInstance::IsSpineRotationAllowed()
{
	return RotationMode != TAG_Status_RotationMode_VelocityDirection;
}

#pragma endregion


#pragma region Look State

void UHumanAnimInstance::ReinitializeLook()
{
	LookState.bReinitializationRequired = true;
}

void UHumanAnimInstance::UpdateLook()
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UHumanAnimInstance::UpdateLook()"), STAT_UHumanAnimInstance_UpdateLook, STATGROUP_Locomotion)

	LookState.bReinitializationRequired |= bPendingUpdate;

	const auto CharacterYawAngle{ UE_REAL_TO_FLOAT(LocomotionState.Rotation.Yaw) };

	if (MovementBase.bHasRelativeRotation)
	{
		// Offset angles to maintain angles relative to the moving base

		LookState.WorldYawAngle = FRotator3f::NormalizeAxis(LookState.WorldYawAngle + MovementBase.DeltaRotation.Yaw);
	}

	float TargetYawAngle;
	float TargetPitchAngle;
	float InterpolationSpeed;

	if (RotationMode == TAG_Status_RotationMode_VelocityDirection)
	{
		// Try to look in the input direction

		TargetYawAngle = FRotator3f::NormalizeAxis((LocomotionState.bHasInput ? LocomotionState.InputYawAngle : LocomotionState.TargetYawAngle) - CharacterYawAngle);

		TargetPitchAngle = 0.0f;
		InterpolationSpeed = LookTowardsInputYawAngleInterpolationSpeed;
	}
	else
	{
		// Try to look in the direction of the view

		TargetYawAngle = ViewState.YawAngle;
		TargetPitchAngle = ViewState.PitchAngle;
		InterpolationSpeed = LookTowardsCameraRotationInterpolationSpeed;
	}

	if (LookState.bReinitializationRequired || InterpolationSpeed <= 0.0f)
	{
		LookState.YawAngle = TargetYawAngle;
		LookState.PitchAngle = TargetPitchAngle;
	}
	else
	{
		const auto YawAngle{ FRotator3f::NormalizeAxis(LookState.WorldYawAngle - CharacterYawAngle) };
		auto DeltaYawAngle{ FRotator3f::NormalizeAxis(TargetYawAngle - YawAngle) };

		if (DeltaYawAngle > 180.0f - ULocomotionFunctionLibrary::CounterClockwiseRotationAngleThreshold)
		{
			DeltaYawAngle -= 360.0f;
		}
		else if (FMath::Abs(LocomotionState.YawSpeed) > UE_SMALL_NUMBER && FMath::Abs(TargetYawAngle) > 90.0f)
		{
			// When interpolating yaw angle, priority is given to the direction of rotation of the character rather than the shortest direction of rotation.
			// Ensure that the rotation of the head and the body are in sync.

			DeltaYawAngle = LocomotionState.YawSpeed > 0.0f ? FMath::Abs(DeltaYawAngle) : -FMath::Abs(DeltaYawAngle);
		}

		const auto InterpolationAmount{ ULocomotionFunctionLibrary::ExponentialDecay(GetDeltaSeconds(), InterpolationSpeed) };

		LookState.YawAngle = FRotator3f::NormalizeAxis(YawAngle + DeltaYawAngle * InterpolationAmount);
		LookState.PitchAngle = ULocomotionFunctionLibrary::LerpAngle(LookState.PitchAngle, TargetPitchAngle, InterpolationAmount);
	}

	LookState.WorldYawAngle = FRotator3f::NormalizeAxis(CharacterYawAngle + LookState.YawAngle);

	// The yaw angle is divided into three separate values. These three values are used to improve the blending of the view when the character is rotated completely around.
	// It is used to improve the blending of views when rotating completely around a character.
	// This allows for smooth blending from left to right or right to left while maintaining view responsiveness.

	LookState.YawForwardAmount = LookState.YawAngle / 360.0f + 0.5f;
	LookState.YawLeftAmount = 0.5f - FMath::Abs(LookState.YawForwardAmount - 0.5f);
	LookState.YawRightAmount = 0.5f + FMath::Abs(LookState.YawForwardAmount - 0.5f);

	LookState.bReinitializationRequired = false;
}

#pragma endregion


#pragma region On Ground State

void UHumanAnimInstance::SetHipsDirection(EHipsDirection NewHipsDirection)
{
	OnGroundState.HipsDirection = NewHipsDirection;
}

void UHumanAnimInstance::ActivatePivot()
{
	OnGroundState.bPivotActivationRequested = true;
}

void UHumanAnimInstance::UpdateGroundedOnGameThread()
{
	check(IsInGameThread());

	OnGroundState.bPivotActive = OnGroundState.bPivotActivationRequested && !bPendingUpdate && (LocomotionState.Speed < PivotActivationSpeedThreshold);

	OnGroundState.bPivotActivationRequested = false;
}

void UHumanAnimInstance::UpdateGrounded(float DeltaTime)
{
	// Always sample the sprint block curve. Failure to do so may cause problems related to inertial blending.

	OnGroundState.SprintBlockAmount = GetCurveValueClamped01(ULocomotionHumanNameStatics::SprintBlockCurveName());
	OnGroundState.HipsDirectionLockAmount = FMath::Clamp(GetCurveValue(ULocomotionHumanNameStatics::HipsDirectionLockCurveName()), -1.0f, 1.0f);

	if (LocomotionMode != TAG_Status_LocomotionMode_OnGround)
	{
		OnGroundState.VelocityBlend.bReinitializationRequired = true;
		OnGroundState.SprintTime = 0.0f;
		return;
	}

	if (!LocomotionState.bMoving)
	{
		ResetGroundedLeanAmount(DeltaTime);
		return;
	}

	// Calculate relative acceleration
	// Indicates the current amount of acceleration/deceleration relative to the character's rotation
	// This value is normalized from -1 to 1, where -1 is the maximum brake deceleration. 
	// 1 equals the maximum acceleration of the character movement component.

	FVector3f RelativeAccelerationAmount;

	if ((LocomotionState.Acceleration | LocomotionState.Velocity) >= 0.0f)
	{
		RelativeAccelerationAmount = ULocomotionFunctionLibrary::ClampMagnitude01(
			FVector3f(LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Acceleration)) / LocomotionState.MaxAcceleration);
	}
	else
	{
		RelativeAccelerationAmount = ULocomotionFunctionLibrary::ClampMagnitude01(
			FVector3f(LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Acceleration)) / LocomotionState.MaxBrakingDeceleration);
	}

	UpdateMovementDirection();
	UpdateVelocityBlend(DeltaTime);
	UpdateRotationYawOffsets();

	UpdateSprint(RelativeAccelerationAmount, DeltaTime);

	UpdateStrideBlendAmount();
	UpdateWalkRunBlendAmount();

	UpdateStandingPlayRate();
	UpdateCrouchingPlayRate();

	UpdateGroundedLeanAmount(RelativeAccelerationAmount, DeltaTime);
}

void UHumanAnimInstance::UpdateMovementDirection()
{
	if (Gait == TAG_Status_Gait_Sprinting)
	{
		OnGroundState.MovementDirection = EMovementDirection::Forward;
		return;
	}

	static constexpr auto ForwardHalfAngle{ 70.0f };

	OnGroundState.MovementDirection = UHumanLocomotionFunctionLibrary::CalculateMovementDirection(
		FRotator3f::NormalizeAxis(UE_REAL_TO_FLOAT(LocomotionState.VelocityYawAngle - ViewState.Rotation.Yaw)),
		ForwardHalfAngle, 5.0f);
}

void UHumanAnimInstance::UpdateVelocityBlend(float DeltaTime)
{
	OnGroundState.VelocityBlend.bReinitializationRequired |= bPendingUpdate;

	const auto RelativeVelocityDirection{ FVector3f{LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Velocity)}.GetSafeNormal() };

	const auto RelativeDirection{ RelativeVelocityDirection / (FMath::Abs(RelativeVelocityDirection.X) + FMath::Abs(RelativeVelocityDirection.Y) + FMath::Abs(RelativeVelocityDirection.Z)) };

	if (OnGroundState.VelocityBlend.bReinitializationRequired)
	{
		OnGroundState.VelocityBlend.bReinitializationRequired = false;

		OnGroundState.VelocityBlend.ForwardAmount	= ULocomotionFunctionLibrary::Clamp01(RelativeDirection.X);
		OnGroundState.VelocityBlend.BackwardAmount	= FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.0f, 0.0f));
		OnGroundState.VelocityBlend.LeftAmount		= FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.0f, 0.0f));
		OnGroundState.VelocityBlend.RightAmount		= ULocomotionFunctionLibrary::Clamp01(RelativeDirection.Y);
	}
	else
	{
		OnGroundState.VelocityBlend.ForwardAmount = FMath::FInterpTo(OnGroundState.VelocityBlend.ForwardAmount,
			ULocomotionFunctionLibrary::Clamp01(RelativeDirection.X), DeltaTime,
			VelocityBlendInterpolationSpeed);

		OnGroundState.VelocityBlend.BackwardAmount = FMath::FInterpTo(OnGroundState.VelocityBlend.BackwardAmount,
			FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.0f, 0.0f)), DeltaTime,
			VelocityBlendInterpolationSpeed);

		OnGroundState.VelocityBlend.LeftAmount = FMath::FInterpTo(OnGroundState.VelocityBlend.LeftAmount,
			FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.0f, 0.0f)), DeltaTime,
			VelocityBlendInterpolationSpeed);

		OnGroundState.VelocityBlend.RightAmount = FMath::FInterpTo(OnGroundState.VelocityBlend.RightAmount,
			ULocomotionFunctionLibrary::Clamp01(RelativeDirection.Y), DeltaTime,
			VelocityBlendInterpolationSpeed);
	}
}

void UHumanAnimInstance::UpdateRotationYawOffsets()
{
	const auto RotationYawOffset{ FRotator3f::NormalizeAxis(UE_REAL_TO_FLOAT(LocomotionState.VelocityYawAngle - ViewState.Rotation.Yaw)) };

	OnGroundState.RotationYawOffsets.ForwardAngle	= RotationYawOffsetForwardCurve->GetFloatValue(RotationYawOffset);
	OnGroundState.RotationYawOffsets.BackwardAngle	= RotationYawOffsetBackwardCurve->GetFloatValue(RotationYawOffset);
	OnGroundState.RotationYawOffsets.LeftAngle		= RotationYawOffsetLeftCurve->GetFloatValue(RotationYawOffset);
	OnGroundState.RotationYawOffsets.RightAngle		= RotationYawOffsetRightCurve->GetFloatValue(RotationYawOffset);
}

void UHumanAnimInstance::UpdateSprint(const FVector3f& RelativeAccelerationAmount, float DeltaTime)
{
	if (Gait != TAG_Status_Gait_Sprinting)
	{
		OnGroundState.SprintTime = 0.0f;
		OnGroundState.SprintAccelerationAmount = 0.0f;
		return;
	}

	static constexpr auto TimeThreshold{ 0.5f };

	OnGroundState.SprintTime = bPendingUpdate ? TimeThreshold : (OnGroundState.SprintTime + DeltaTime);
	OnGroundState.SprintAccelerationAmount = (OnGroundState.SprintTime >= TimeThreshold) ? 0.0f : RelativeAccelerationAmount.X;
}

void UHumanAnimInstance::UpdateStrideBlendAmount()
{
	const auto Speed{ LocomotionState.Speed / LocomotionState.Scale };

	const auto StandingStrideBlend{ FMath::Lerp(StrideBlendAmountWalkCurve->GetFloatValue(Speed), StrideBlendAmountRunCurve->GetFloatValue(Speed), PoseState.UnweightedGaitRunningAmount) };

	// The amount of blend in the crouched stride.

	OnGroundState.StrideBlendAmount = FMath::Lerp(StandingStrideBlend, StrideBlendAmountWalkCurve->GetFloatValue(Speed), PoseState.CrouchingAmount);
}

void UHumanAnimInstance::UpdateWalkRunBlendAmount()
{
	// Calculates the Walk / Run blend volume. This value is used within the blend space to blend walking and running.

	OnGroundState.WalkRunBlendAmount = (Gait == TAG_Status_Gait_Walking) ? 0.0f : 1.0f;
}

void UHumanAnimInstance::UpdateStandingPlayRate()
{
	// Calculate the standing play rate by dividing the character's speed by the animated speed of each walk.

	const auto WalkRunSpeedAmount
	{ 
		FMath::Lerp(LocomotionState.Speed / AnimatedWalkSpeed, LocomotionState.Speed / AnimatedRunSpeed, PoseState.UnweightedGaitRunningAmount)
	};

	const auto WalkRunSprintSpeedAmount
	{ 
		FMath::Lerp(WalkRunSpeedAmount, LocomotionState.Speed / AnimatedSprintSpeed, PoseState.UnweightedGaitSprintingAmount)
	};

	OnGroundState.StandingPlayRate = FMath::Clamp(WalkRunSprintSpeedAmount / (OnGroundState.StrideBlendAmount * LocomotionState.Scale), 0.0f, 3.0f);
}

void UHumanAnimInstance::UpdateCrouchingPlayRate()
{
	OnGroundState.CrouchingPlayRate = FMath::Clamp(LocomotionState.Speed / (AnimatedCrouchSpeed * OnGroundState.StrideBlendAmount * LocomotionState.Scale), 0.0f, 2.0f);
}

void UHumanAnimInstance::UpdateGroundedLeanAmount(const FVector3f& RelativeAccelerationAmount, float DeltaTime)
{
	if (bPendingUpdate)
	{
		LeanState.RightAmount = RelativeAccelerationAmount.Y;
		LeanState.ForwardAmount = RelativeAccelerationAmount.X;
	}
	else
	{
		LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, RelativeAccelerationAmount.Y, DeltaTime, LeanInterpolationSpeed);
		LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, RelativeAccelerationAmount.X, DeltaTime, LeanInterpolationSpeed);
	}
}

void UHumanAnimInstance::ResetGroundedLeanAmount(float DeltaTime)
{
	if (bPendingUpdate)
	{
		LeanState.RightAmount = 0.0f;
		LeanState.ForwardAmount = 0.0f;
	}
	else
	{
		LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, 0.0f, DeltaTime, LeanInterpolationSpeed);
		LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, 0.0f, DeltaTime, LeanInterpolationSpeed);
	}
}

#pragma endregion


#pragma region InAir

void UHumanAnimInstance::ResetJumped()
{
	InAirState.bJumped = false;
}

void UHumanAnimInstance::UpdateInAirOnGameThread()
{
	check(IsInGameThread());

	InAirState.bJumped = !bPendingUpdate && (InAirState.bJumped || (InAirState.VerticalVelocity > 0));
}

void UHumanAnimInstance::UpdateInAir(float DeltaTime)
{
	if (LocomotionMode != TAG_Status_LocomotionMode_InAir)
	{
		return;
	}

	if (InAirState.bJumped)
	{
		static constexpr auto ReferenceSpeed{ 600.0f };
		static constexpr auto MinPlayRate{ 1.2f };
		static constexpr auto MaxPlayRate{ 1.5f };

		InAirState.JumpPlayRate = ULocomotionFunctionLibrary::LerpClamped(MinPlayRate, MaxPlayRate, LocomotionState.Speed / ReferenceSpeed);
	}

	// Caches the vertical velocity and determines the speed at which the character lands on the ground

	InAirState.VerticalVelocity = UE_REAL_TO_FLOAT(LocomotionState.Velocity.Z);

	UpdateGroundPredictionAmount();

	UpdateInAirLeanAmount(DeltaTime);
}

void UHumanAnimInstance::UpdateGroundPredictionAmount()
{
	// Calculate the predicted weight of the ground by tracing in the direction of velocity and finding a surface on which the character can walk.

	static constexpr auto VerticalVelocityThreshold{ -200.0f };
	static constexpr auto MinVerticalVelocity{ -4000.0f };
	static constexpr auto MaxVerticalVelocity{ -200.0f };
	static constexpr auto MinSweepDistance{ 150.0f };
	static constexpr auto MaxSweepDistance{ 2000.0f };

	if (InAirState.VerticalVelocity > VerticalVelocityThreshold)
	{
		InAirState.GroundPredictionAmount = 0.0f;
		return;
	}

	const auto AllowanceAmount{ 1.0f - GetCurveValueClamped01(ULocomotionGeneralNameStatics::GroundPredictionBlockCurveName()) };

	if (AllowanceAmount <= UE_KINDA_SMALL_NUMBER)
	{
		InAirState.GroundPredictionAmount = 0.0f;
		return;
	}

	const auto SweepStartLocation{ LocomotionState.Location };

	auto VelocityDirection{ LocomotionState.Velocity };

	VelocityDirection.Z = FMath::Clamp(VelocityDirection.Z, MinVerticalVelocity, MaxVerticalVelocity);
	VelocityDirection.Normalize();

	const auto SweepVector{ VelocityDirection * FMath::GetMappedRangeValueClamped(FVector2f(MaxVerticalVelocity, MinVerticalVelocity), FVector2f(MinSweepDistance, MaxSweepDistance), InAirState.VerticalVelocity) * LocomotionState.Scale };

	FHitResult Hit;
	GetWorld()->SweepSingleByChannel(
		Hit, 
		SweepStartLocation, 
		SweepStartLocation + SweepVector, 
		FQuat::Identity, 
		ECC_WorldStatic,
		FCollisionShape::MakeCapsule(LocomotionState.CapsuleRadius, LocomotionState.CapsuleHalfHeight),
		FCollisionQueryParams(__FUNCTION__, false, Character), 
		GroundPredictionSweepResponses);
	
	const auto bGroundValid{ Hit.IsValidBlockingHit() && (Hit.ImpactNormal.Z >= LocomotionState.WalkableFloorZ) };

	InAirState.GroundPredictionAmount = bGroundValid ? (GroundPredictionAmountCurve->GetFloatValue(Hit.Time) * AllowanceAmount) : 0.0f;
}

void UHumanAnimInstance::UpdateInAirLeanAmount(float DeltaTime)
{
	// Use the direction and amount of relative velocity to determine how much the character will tilt

	static constexpr auto ReferenceSpeed{ 350.0f };

	const auto RelativeVelocity
	{ 
		FVector3f(LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Velocity)) / ReferenceSpeed * LeanAmountCurve->GetFloatValue(InAirState.VerticalVelocity)
	};

	if (bPendingUpdate)
	{
		LeanState.RightAmount = RelativeVelocity.Y;
		LeanState.ForwardAmount = RelativeVelocity.X;
	}
	else
	{
		LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, RelativeVelocity.Y, DeltaTime, LeanInterpolationSpeed);

		LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, RelativeVelocity.X, DeltaTime, LeanInterpolationSpeed);
	}
}

#pragma endregion


#pragma region Feet

void UHumanAnimInstance::UpdateFeetOnGameThread()
{
	check(IsInGameThread());

	const auto* Mesh{ GetSkelMeshComponent() };

	const auto FootLeftTargetTransform
	{
		Mesh->GetSocketTransform(bUseFootIkBones
									 ? ULocomotionHumanNameStatics::FootLeftIkBoneName()
									 : ULocomotionHumanNameStatics::FootLeftVirtualBoneName())
	};

	FeetState.Left.TargetLocation = FootLeftTargetTransform.GetLocation();
	FeetState.Left.TargetRotation = FootLeftTargetTransform.GetRotation();

	const auto FootRightTargetTransform
	{
		Mesh->GetSocketTransform(bUseFootIkBones
									 ? ULocomotionHumanNameStatics::FootRightIkBoneName()
									 : ULocomotionHumanNameStatics::FootRightVirtualBoneName())
	};

	FeetState.Right.TargetLocation = FootRightTargetTransform.GetLocation();
	FeetState.Right.TargetRotation = FootRightTargetTransform.GetRotation();
}

void UHumanAnimInstance::UpdateFeet(float DeltaTime)
{
	FeetState.FootPlantedAmount = FMath::Clamp(GetCurveValue(ULocomotionHumanNameStatics::FootPlantedCurveName()), -1.0f, 1.0f);
	FeetState.FeetCrossingAmount = GetCurveValueClamped01(ULocomotionHumanNameStatics::FeetCrossingCurveName());

	FeetState.MinMaxPelvisOffsetZ = FVector2D::ZeroVector;

	const auto ComponentTransformInverse{ GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform().Inverse() };

	UpdateFoot(FeetState.Left, ULocomotionHumanNameStatics::FootLeftIkCurveName(),
								ULocomotionHumanNameStatics::FootLeftLockCurveName(), ComponentTransformInverse, DeltaTime);

	UpdateFoot(FeetState.Right, ULocomotionHumanNameStatics::FootRightIkCurveName(),
								 ULocomotionHumanNameStatics::FootRightLockCurveName(), ComponentTransformInverse, DeltaTime);

	FeetState.MinMaxPelvisOffsetZ.X = FMath::Min(FeetState.Left.OffsetTargetLocation.Z, FeetState.Right.OffsetTargetLocation.Z) /
												 LocomotionState.Scale;

	FeetState.MinMaxPelvisOffsetZ.Y = FMath::Max(FeetState.Left.OffsetTargetLocation.Z, FeetState.Right.OffsetTargetLocation.Z) /
												 LocomotionState.Scale;
}

void UHumanAnimInstance::UpdateFoot(FFootState& FootState, const FName& FootIkCurveName, const FName& FootLockCurveName, const FTransform& ComponentTransformInverse, float DeltaTime) const
{
	FootState.IkAmount = GetCurveValueClamped01(FootIkCurveName);

	ProcessFootLockTeleport(FootState);

	ProcessFootLockBaseChange(FootState, ComponentTransformInverse);

	auto FinalLocation{ FootState.TargetLocation };
	auto FinalRotation{ FootState.TargetRotation };

	UpdateFootLock(FootState, FootLockCurveName, ComponentTransformInverse, DeltaTime, FinalLocation, FinalRotation);

	UpdateFootOffset(FootState, DeltaTime, FinalLocation, FinalRotation);

	FootState.IkLocation = ComponentTransformInverse.TransformPosition(FinalLocation);
	FootState.IkRotation = ComponentTransformInverse.TransformRotation(FinalRotation);
}

void UHumanAnimInstance::ProcessFootLockTeleport(FFootState& FootState) const
{
	// Assume that teleportation occurs in a short period of time due to network smoothing.

	if (bPendingUpdate || GetWorld()->TimeSince(TeleportedTime) > 0.2f ||
		!FAnimWeight::IsRelevant(FootState.IkAmount * FootState.LockAmount))
	{
		return;
	}

	const auto& ComponentTransform{ GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform() };

	FootState.LockLocation = ComponentTransform.TransformPosition(FootState.LockComponentRelativeLocation);
	FootState.LockRotation = ComponentTransform.TransformRotation(FootState.LockComponentRelativeRotation);

	if (MovementBase.bHasRelativeLocation)
	{
		const auto BaseRotationInverse{ MovementBase.Rotation.Inverse() };

		FootState.LockMovementBaseRelativeLocation = BaseRotationInverse.RotateVector(FootState.LockLocation - MovementBase.Location);
		FootState.LockMovementBaseRelativeRotation = BaseRotationInverse * FootState.LockRotation;
	}
}

void UHumanAnimInstance::ProcessFootLockBaseChange(FFootState& FootState, const FTransform& ComponentTransformInverse) const
{
	if ((!bPendingUpdate && !MovementBase.bBaseChanged) || !FAnimWeight::IsRelevant(FootState.IkAmount * FootState.LockAmount))
	{
		return;
	}

	if (bPendingUpdate)
	{
		FootState.LockLocation = FootState.TargetLocation;
		FootState.LockRotation = FootState.TargetRotation;
	}

	FootState.LockComponentRelativeLocation = ComponentTransformInverse.TransformPosition(FootState.LockLocation);
	FootState.LockComponentRelativeRotation = ComponentTransformInverse.TransformRotation(FootState.LockRotation);

	if (MovementBase.bHasRelativeLocation)
	{
		const auto BaseRotationInverse{ MovementBase.Rotation.Inverse() };

		FootState.LockMovementBaseRelativeLocation = BaseRotationInverse.RotateVector(FootState.LockLocation - MovementBase.Location);
		FootState.LockMovementBaseRelativeRotation = BaseRotationInverse * FootState.LockRotation;
	}
	else
	{
		FootState.LockMovementBaseRelativeLocation = FVector::ZeroVector;
		FootState.LockMovementBaseRelativeRotation = FQuat::Identity;
	}
}

void UHumanAnimInstance::UpdateFootLock(FFootState& FootState, const FName& FootLockCurveName, const FTransform& ComponentTransformInverse, float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const
{
	auto NewFootLockAmount{ GetCurveValueClamped01(FootLockCurveName) };

	NewFootLockAmount *= 1.0f - RotateInPlaceState.FootLockBlockAmount;

	if (LocomotionState.bMovingSmooth || LocomotionMode != TAG_Status_LocomotionMode_OnGround)
	{
		// Smoothly disables leg locks when the character is moving or in the air.

		static constexpr auto MovingDecreaseSpeed{ 5.0f };
		static constexpr auto NotGroundedDecreaseSpeed{ 0.6f };

		NewFootLockAmount = bPendingUpdate
			? 0.0f
			: FMath::Max(
				0.0f, 
				FMath::Min(
					NewFootLockAmount,
					FootState.LockAmount - DeltaTime * (LocomotionState.bMovingSmooth ? MovingDecreaseSpeed : NotGroundedDecreaseSpeed)
				)
			);
	}

	if (bDisableFootLock || !FAnimWeight::IsRelevant(FootState.IkAmount * NewFootLockAmount))
	{
		if (FootState.LockAmount > 0.0f)
		{
			FootState.LockAmount = 0.0f;

			FootState.LockLocation = FVector::ZeroVector;
			FootState.LockRotation = FQuat::Identity;

			FootState.LockComponentRelativeLocation = FVector::ZeroVector;
			FootState.LockComponentRelativeRotation = FQuat::Identity;

			FootState.LockMovementBaseRelativeLocation = FVector::ZeroVector;
			FootState.LockMovementBaseRelativeRotation = FQuat::Identity;
		}

		return;
	}

	const auto bNewAmountEqualOne{ FAnimWeight::IsFullWeight(NewFootLockAmount) };
	const auto bNewAmountGreaterThanPrevious{ NewFootLockAmount > FootState.LockAmount };

	// Update the footlocker amount only if the new amount is less than or equal to 1 of the current amount.

	if (bNewAmountEqualOne)
	{
		if (bNewAmountGreaterThanPrevious)
		{
			if (FootState.LockAmount <= 0.9f)
			{
				// Maintains the same locking position and rotation as the last time it was locked.

				FootState.LockLocation = FinalLocation;
				FootState.LockRotation = FinalRotation;
			}

			if (MovementBase.bHasRelativeLocation)
			{
				const auto BaseRotationInverse{ MovementBase.Rotation.Inverse() };

				FootState.LockMovementBaseRelativeLocation = BaseRotationInverse.RotateVector(FinalLocation - MovementBase.Location);
				FootState.LockMovementBaseRelativeRotation = BaseRotationInverse * FinalRotation;
			}
			else
			{
				FootState.LockMovementBaseRelativeLocation = FVector::ZeroVector;
				FootState.LockMovementBaseRelativeRotation = FQuat::Identity;
			}
		}

		FootState.LockAmount = 1.0f;
	}
	else if (!bNewAmountGreaterThanPrevious)
	{
		FootState.LockAmount = NewFootLockAmount;
	}

	if (MovementBase.bHasRelativeLocation)
	{
		FootState.LockLocation = MovementBase.Location + MovementBase.Rotation.RotateVector(FootState.LockMovementBaseRelativeLocation);
		FootState.LockRotation = MovementBase.Rotation * FootState.LockMovementBaseRelativeRotation;
	}

	FootState.LockComponentRelativeLocation = ComponentTransformInverse.TransformPosition(FootState.LockLocation);
	FootState.LockComponentRelativeRotation = ComponentTransformInverse.TransformRotation(FootState.LockRotation);

	FinalLocation = FMath::Lerp(FinalLocation, FootState.LockLocation, FootState.LockAmount);
	FinalRotation = FQuat::Slerp(FinalRotation, FootState.LockRotation, FootState.LockAmount);
}

void UHumanAnimInstance::UpdateFootOffset(FFootState& FootState, float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const
{
	if (!FAnimWeight::IsRelevant(FootState.IkAmount))
	{
		FootState.OffsetTargetLocation = FVector::ZeroVector;
		FootState.OffsetTargetRotation = FQuat::Identity;
		FootState.OffsetSpringState.Reset();
		return;
	}

	if (LocomotionMode == TAG_Status_LocomotionMode_InAir)
	{
		FootState.OffsetTargetLocation = FVector::ZeroVector;
		FootState.OffsetTargetRotation = FQuat::Identity;
		FootState.OffsetSpringState.Reset();

		if (bPendingUpdate)
		{
			FootState.OffsetLocation = FVector::ZeroVector;
			FootState.OffsetRotation = FQuat::Identity;
		}
		else
		{
			static constexpr auto InterpolationSpeed{ 15.0f };

			FootState.OffsetLocation = FMath::VInterpTo(FootState.OffsetLocation, FVector::ZeroVector, DeltaTime, InterpolationSpeed);
			FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, FQuat::Identity, DeltaTime, InterpolationSpeed);

			FinalLocation += FootState.OffsetLocation;
			FinalRotation = FootState.OffsetRotation * FinalRotation;
		}

		return;
	}

	// Trace down from the foot location to find the geometry. If the surface is walkable, save the impact location and normals

	const FVector TraceLocation{ FinalLocation.X, FinalLocation.Y, GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform().GetLocation().Z };

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceLocation + FVector(0.0f, 0.0f, IkTraceDistanceUpward* LocomotionState.Scale),
		TraceLocation - FVector(0.0f, 0.0f, IkTraceDistanceDownward * LocomotionState.Scale),
		UEngineTypes::ConvertToCollisionChannel(IkTraceChannel),
		FCollisionQueryParams(__FUNCTION__, true, Character));

	const auto bGroundValid{ Hit.IsValidBlockingHit() && Hit.ImpactNormal.Z >= LocomotionState.WalkableFloorZ };

	if (bGroundValid)
	{
		const auto ActualFootHeight{ FootHeight * LocomotionState.Scale };

		// Find the difference in position between the impact location and the expected (flat) floor location.

		FootState.OffsetTargetLocation = Hit.ImpactPoint - TraceLocation + Hit.ImpactNormal * ActualFootHeight;
		FootState.OffsetTargetLocation.Z -= ActualFootHeight;

		// Calculate rotational offset

		FootState.OffsetTargetRotation = FRotator(
			-ULocomotionFunctionLibrary::DirectionToAngle(FVector2D(Hit.ImpactNormal.Z, Hit.ImpactNormal.X)),
			0.0f,
			ULocomotionFunctionLibrary::DirectionToAngle(FVector2D(Hit.ImpactNormal.Z, Hit.ImpactNormal.Y))).Quaternion();
	}

	// Interpolate current offset to new target value

	if (bPendingUpdate)
	{
		FootState.OffsetSpringState.Reset();

		FootState.OffsetLocation = FootState.OffsetTargetLocation;
		FootState.OffsetRotation = FootState.OffsetTargetRotation;
	}
	else
	{
		static constexpr auto LocationInterpolationFrequency{ 0.4f };
		static constexpr auto LocationInterpolationDampingRatio{ 4.0f };
		static constexpr auto LocationInterpolationTargetVelocityAmount{ 1.0f };

		FootState.OffsetLocation = UHumanLocomotionFunctionLibrary::SpringDampVector(FootState.OffsetLocation, FootState.OffsetTargetLocation,
			FootState.OffsetSpringState, DeltaTime, LocationInterpolationFrequency,
			LocationInterpolationDampingRatio, LocationInterpolationTargetVelocityAmount);

		static constexpr auto RotationInterpolationSpeed{ 30.0f };

		FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, FootState.OffsetTargetRotation,
			DeltaTime, RotationInterpolationSpeed);
	}

	FinalLocation += FootState.OffsetLocation;
	FinalRotation = FootState.OffsetRotation * FinalRotation;
}

#pragma endregion


#pragma region Transitions

void UHumanAnimInstance::PlayQuickStopAnimation()
{
	if (RotationMode != TAG_Status_RotationMode_VelocityDirection)
	{
		PlayTransitionLeftAnimation(QuickStopBlendInDuration, QuickStopBlendOutDuration, QuickStopPlayRate.X, QuickStopStartTime);
		return;
	}

	auto RotationYawAngle{ FRotator3f::NormalizeAxis(UE_REAL_TO_FLOAT((LocomotionState.bHasInput ? LocomotionState.InputYawAngle : LocomotionState.TargetYawAngle) - LocomotionState.Rotation.Yaw)) };

	if (RotationYawAngle > 180.0f - ULocomotionFunctionLibrary::CounterClockwiseRotationAngleThreshold)
	{
		RotationYawAngle -= 360.0f;
	}

	// Adjust the playback speed of the quick stop animation based on the distance of the character

	if (RotationYawAngle <= 0.0f)
	{
		PlayTransitionLeftAnimation(QuickStopBlendInDuration, QuickStopBlendOutDuration,
			FMath::Lerp(QuickStopPlayRate.X, QuickStopPlayRate.Y,
				FMath::Abs(RotationYawAngle) / 180.0f), QuickStopStartTime);
	}
	else
	{
		PlayTransitionRightAnimation(QuickStopBlendInDuration, QuickStopBlendOutDuration,
			FMath::Lerp(QuickStopPlayRate.X, QuickStopPlayRate.Y,
				FMath::Abs(RotationYawAngle) / 180.0f), QuickStopStartTime);
	}
}

void UHumanAnimInstance::PlayTransitionAnimation(UAnimSequenceBase* Animation, float BlendInDuration, float BlendOutDuration, float PlayRate, float StartTime, bool bFromStandingIdleOnly)
{
	check(IsInGameThread());

	if (!IsValid(CharacterMovement))
	{
		return;
	}

	if (bFromStandingIdleOnly && (CharacterMovement->GetLocomotionState().bMoving || CharacterMovement->GetStance() != TAG_Status_Stance_Standing))
	{
		return;
	}

	PlaySlotAnimationAsDynamicMontage(Animation, ULocomotionHumanNameStatics::TransitionSlotName(), BlendInDuration, BlendOutDuration, PlayRate, 1, 0.0f, StartTime);
}

void UHumanAnimInstance::PlayTransitionLeftAnimation(float BlendInDuration, float BlendOutDuration, float PlayRate, float StartTime, bool bFromStandingIdleOnly)
{
	PlayTransitionAnimation(Stance == TAG_Status_Stance_Crouching
		? CrouchingTransitionLeftAnimation
		: StandingTransitionLeftAnimation,
		BlendInDuration, BlendOutDuration, PlayRate, StartTime, bFromStandingIdleOnly);
}

void UHumanAnimInstance::PlayTransitionRightAnimation(float BlendInDuration, float BlendOutDuration, float PlayRate, float StartTime, bool bFromStandingIdleOnly)
{
	PlayTransitionAnimation(Stance == TAG_Status_Stance_Crouching
		? CrouchingTransitionRightAnimation
		: StandingTransitionRightAnimation,
		BlendInDuration, BlendOutDuration, PlayRate, StartTime, bFromStandingIdleOnly);
}

void UHumanAnimInstance::StopTransitionAndTurnInPlaceAnimations(float BlendOutDuration)
{
	check(IsInGameThread());

	StopSlotAnimation(BlendOutDuration, ULocomotionHumanNameStatics::TransitionSlotName());
}

void UHumanAnimInstance::UpdateTransitions()
{
	// Because the allowed transition curve changes within certain states, the allowed transitions are true in those states.

	TransitionsState.bTransitionsAllowed = FAnimWeight::IsFullWeight(GetCurveValue(ULocomotionGeneralNameStatics::AllowTransitionsCurveName()));

	UpdateDynamicTransition();
}

void UHumanAnimInstance::UpdateDynamicTransition()
{
	if (TransitionsState.DynamicTransitionsFrameDelay > 0)
	{
		TransitionsState.DynamicTransitionsFrameDelay -= 1;
		return;
	}

	if (!TransitionsState.bTransitionsAllowed || LocomotionState.bMoving || LocomotionMode != TAG_Status_LocomotionMode_OnGround)
	{
		return;
	}

	// Check each foot to see the difference between the appearance of the foot and its position relative to its desired/target position.

	const auto FootLockDistanceThresholdSquared{ FMath::Square(DynamicTransitionFootLockDistanceThreshold * LocomotionState.Scale) };

	const auto FootLockLeftDistanceSquared{ FVector::DistSquared(FeetState.Left.TargetLocation, FeetState.Left.LockLocation) };
	const auto FootLockRightDistanceSquared{ FVector::DistSquared(FeetState.Right.TargetLocation, FeetState.Right.LockLocation) };

	const auto bTransitionLeftAllowed{ FAnimWeight::IsRelevant(FeetState.Left.LockAmount) && FootLockLeftDistanceSquared > FootLockDistanceThresholdSquared };
	const auto bTransitionRightAllowed{ FAnimWeight::IsRelevant(FeetState.Right.LockAmount) && FootLockRightDistanceSquared > FootLockDistanceThresholdSquared };

	if (!bTransitionLeftAllowed && !bTransitionRightAllowed)
	{
		return;
	}

	TObjectPtr<UAnimSequenceBase> DynamicTransitionAnimation;

	// If both transitions are allowed, select the one with the greater locking distance.

	if (!bTransitionLeftAllowed)
	{
		DynamicTransitionAnimation = Stance == TAG_Status_Stance_Crouching
			? CrouchingDynamicTransitionRightAnimation
			: StandingDynamicTransitionRightAnimation;
	}
	else if (!bTransitionRightAllowed)
	{
		DynamicTransitionAnimation = Stance == TAG_Status_Stance_Crouching
			? CrouchingDynamicTransitionLeftAnimation
			: StandingDynamicTransitionLeftAnimation;
	}
	else if (FootLockLeftDistanceSquared >= FootLockRightDistanceSquared)
	{
		DynamicTransitionAnimation = Stance == TAG_Status_Stance_Crouching
			? CrouchingDynamicTransitionLeftAnimation
			: StandingDynamicTransitionLeftAnimation;
	}
	else
	{
		DynamicTransitionAnimation = Stance == TAG_Status_Stance_Crouching
			? CrouchingDynamicTransitionRightAnimation
			: StandingDynamicTransitionRightAnimation;
	}

	if (IsValid(DynamicTransitionAnimation))
	{
		// Block the next dynamic transition by approximately two frames to give the animation blueprint time to react properly to the animation.

		TransitionsState.DynamicTransitionsFrameDelay = 2;

		// Animated montages cannot be played in the worker thread, so they are queued and played later in the game thread.

		TransitionsState.QueuedDynamicTransitionAnimation = DynamicTransitionAnimation;

		if (IsInGameThread())
		{
			PlayQueuedDynamicTransitionAnimation();
		}
	}
}

void UHumanAnimInstance::PlayQueuedDynamicTransitionAnimation()
{
	check(IsInGameThread());

	PlaySlotAnimationAsDynamicMontage(TransitionsState.QueuedDynamicTransitionAnimation, ULocomotionHumanNameStatics::TransitionSlotName(),
		DynamicTransitionBlendDuration,
		DynamicTransitionBlendDuration,
		DynamicTransitionPlayRate, 1, 0.0f);

	TransitionsState.QueuedDynamicTransitionAnimation = nullptr;
}

#pragma endregion


#pragma region Rotate In Place

void UHumanAnimInstance::UpdateRotateInPlace(float DeltaTime)
{
	static constexpr auto PlayRateInterpolationSpeed{ 5.0f };

	// Rotation in place is only permitted when the character is stationary and aiming, or in first-person view mode.

	if (LocomotionState.bMoving || LocomotionMode != TAG_Status_LocomotionMode_OnGround || !IsRotateInPlaceAllowed())
	{
		RotateInPlaceState.bRotatingLeft = false;
		RotateInPlaceState.bRotatingRight = false;

		RotateInPlaceState.PlayRate = bPendingUpdate
			? RotationInPlacePlayRate.X
			: FMath::FInterpTo(RotateInPlaceState.PlayRate, RotationInPlacePlayRate.X,
				DeltaTime, PlayRateInterpolationSpeed);

		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// Check if the yaw angle of the view exceeds the threshold to see if the character should be rotated left or right.

	RotateInPlaceState.bRotatingLeft = ViewState.YawAngle < -ViewYawAngleThreshold;
	RotateInPlaceState.bRotatingRight = ViewState.YawAngle > ViewYawAngleThreshold;

	if (!RotateInPlaceState.bRotatingLeft && !RotateInPlaceState.bRotatingRight)
	{
		RotateInPlaceState.PlayRate = bPendingUpdate
			? RotationInPlacePlayRate.X
			: FMath::FInterpTo(RotateInPlaceState.PlayRate, RotationInPlacePlayRate.X,
				DeltaTime, PlayRateInterpolationSpeed);

		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// If the character needs to be rotated, set the playback rate to match the yaw of the view.

	const auto PlayRate{ FMath::GetMappedRangeValueClamped(ReferenceViewYawSpeed, RotationInPlacePlayRate, ViewState.YawSpeed) };

	RotateInPlaceState.PlayRate = bPendingUpdate
		? PlayRate
		: FMath::FInterpTo(RotateInPlaceState.PlayRate, PlayRate,
			DeltaTime, PlayRateInterpolationSpeed);

	// Disable the foot lock when rotating at large angles or rotating too fast. Otherwise, the legs may twist spirally.

	static constexpr auto BlockInterpolationSpeed{ 5.0f };

	RotateInPlaceState.FootLockBlockAmount =
		bDisableFootLock
		? 1.0f
		: FMath::Abs(ViewState.YawAngle) > FootLockBlockViewYawAngleThreshold
		? 0.5f
		: ViewState.YawSpeed <= FootLockBlockViewYawSpeedThreshold
		? 0.0f
		: bPendingUpdate
		? 1.0f
		: FMath::FInterpTo(RotateInPlaceState.FootLockBlockAmount, 1.0f, DeltaTime, BlockInterpolationSpeed);
}

bool UHumanAnimInstance::IsRotateInPlaceAllowed()
{
	return RotationMode != TAG_Status_RotationMode_VelocityDirection;
}

#pragma endregion


#pragma region Control Rig Input

FControlRigInput UHumanAnimInstance::GetControlRigInput() const
{
	return {
		bUseHandIkBones,
		bUseFootIkBones,
		OnGroundState.VelocityBlend.ForwardAmount,
		OnGroundState.VelocityBlend.BackwardAmount,
		SpineRotationState.YawAngle,
		FeetState.Left.IkRotation,
		FeetState.Left.IkLocation,
		FeetState.Left.IkAmount,
		FeetState.Right.IkRotation,
		FeetState.Right.IkLocation,
		FeetState.Right.IkAmount,
		FeetState.MinMaxPelvisOffsetZ,
	};
}

#pragma endregion
