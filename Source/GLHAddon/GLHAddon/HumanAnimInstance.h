// Copyright (C) 2024 owoDra

#pragma once

#include "CharacterAnimInstance.h"

#include "State/LayeringState.h"
#include "State/PoseState.h"
#include "State/SpineRotationState.h"
#include "State/LookState.h"
#include "State/LeanState.h"
#include "State/OnGroundState.h"
#include "State/InAirState.h"
#include "State/InWaterState.h"
#include "State/FeetState.h"
#include "State/TransitionsState.h"
#include "State/RotateInPlaceState.h"
#include "State/ControlRigInput.h"

#include "HumanAnimInstance.generated.h"

class UHumanLinkedAnimInstance;


/**
 * Main AnimInsntace class specialized for human character features
 * 
 * Tips:
 *	Basically, it is used only for TPP Mesh of Character and processes data necessary for animation.
 */
UCLASS(Config = Game)
class GLHADDON_API UHumanAnimInstance : public UCharacterAnimInstance
{
	GENERATED_BODY()

	friend UHumanLinkedAnimInstance;

public:
	UHumanAnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;


protected:
	virtual void UpdateAnimationOnGameThread(float DeltaTime) override;
	virtual void UpdateAnimationOnThreadSafe(float DeltaTime) override;
	virtual void OnPostEvaluateAnimation() override;


	//////////////////////////////////////////////////////////////
	// Layering State
#pragma region Layering State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FLayeringState LayeringState;

protected:
	void UpdateLayering();

#pragma endregion


	//////////////////////////////////////////////////////////////
	// Pose State
#pragma region Pose State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FPoseState PoseState;

protected:
	void UpdatePose();

#pragma endregion


	/////////////////////////////////////////
	// Spine Rotation State
#pragma region Spine Rotation State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FSpineRotationState SpineRotationState;

protected:
	void UpdateSpineRotation(float DeltaTime);

public:
	virtual bool IsSpineRotationAllowed();

#pragma endregion


	/////////////////////////////////////////
	// Look State
#pragma region Look State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FLookState LookState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Look", Meta = (ClampMin = 0))
	float LookTowardsCameraRotationInterpolationSpeed{ 8.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Look", Meta = (ClampMin = 0))
	float LookTowardsInputYawAngleInterpolationSpeed{ 8.0f };

protected:
	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void ReinitializeLook();

	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void UpdateLook();

#pragma endregion


	/////////////////////////////////////////
	// Lean State
#pragma region Lean State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FLeanState LeanState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Lean", Meta = (ClampMin = 0))
	float LeanInterpolationSpeed{ 4.0f };

#pragma endregion


	/////////////////////////////////////////
	// On Ground State
#pragma region On Ground State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FOnGroundState OnGroundState;

	// 
	// Blend Amount Curves for Travel Speed and Stride
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround")
	TObjectPtr<UCurveFloat> StrideBlendAmountWalkCurve{ nullptr };

	// 
	// Blend Amount Curves for Travel Speed and Stride
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround")
	TObjectPtr<UCurveFloat> StrideBlendAmountRunCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround")
	TObjectPtr<UCurveFloat> RotationYawOffsetForwardCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround")
	TObjectPtr<UCurveFloat> RotationYawOffsetBackwardCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround")
	TObjectPtr<UCurveFloat> RotationYawOffsetLeftCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround")
	TObjectPtr<UCurveFloat> RotationYawOffsetRightCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround", Meta = (ClampMin = 0))
	float VelocityBlendInterpolationSpeed{ 12.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float PivotActivationSpeedThreshold{ 200.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedWalkSpeed{ 150.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedRunSpeed{ 350.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedSprintSpeed{ 600.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|OnGround", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedCrouchSpeed{ 150.0f };

protected:
	void UpdateGroundedOnGameThread();

	void UpdateGrounded(float DeltaTime);

	void UpdateMovementDirection();

	void UpdateVelocityBlend(float DeltaTime);

	void UpdateRotationYawOffsets();

	void UpdateSprint(const FVector3f& RelativeAccelerationAmount, float DeltaTime);

	void UpdateStrideBlendAmount();

	void UpdateWalkRunBlendAmount();

	void UpdateStandingPlayRate();

	void UpdateCrouchingPlayRate();

	void UpdateGroundedLeanAmount(const FVector3f& RelativeAccelerationAmount, float DeltaTime);

	void ResetGroundedLeanAmount(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void SetHipsDirection(EHipsDirection NewHipsDirection);

	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void ActivatePivot();

#pragma endregion


	/////////////////////////////////////////
	// In Air State
#pragma region In Air State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FInAirState InAirState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|InAir")
	TObjectPtr<UCurveFloat> LeanAmountCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|InAir")
	TObjectPtr<UCurveFloat> GroundPredictionAmountCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|InAir")
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundPredictionSweepObjectTypes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Configs|InAir")
	FCollisionResponseContainer GroundPredictionSweepResponses;

protected:
	void UpdateInAirOnGameThread();

	void UpdateInAir(float DeltaTime);

	void UpdateGroundPredictionAmount();

	void UpdateInAirLeanAmount(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void ResetJumped();
	
#pragma endregion


	/////////////////////////////////////////
	// In Water State
#pragma region In Water State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FInWaterState InWaterState;

protected:
	void UpdateInWaterOnGameThread() {}

	void UpdateInWater(float DeltaTime) {}

	void UpdateInWaterLeanAmount(float DeltaTime) {}

#pragma endregion
	

	/////////////////////////////////////////
	// Feet State
#pragma region Feet State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FFeetState FeetState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Feet")
	bool bUseFootIkBones{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Feet")
	bool bDisableFootLock{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Feet", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float FootHeight{ 13.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Feet")
	TEnumAsByte<ETraceTypeQuery> IkTraceChannel{ TraceTypeQuery1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Feet", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float IkTraceDistanceUpward{ 50.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Feet", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float IkTraceDistanceDownward{ 45.0f };

protected:
	void UpdateFeetOnGameThread();

	void UpdateFeet(float DeltaTime);

	void UpdateFoot(FFootState& FootState, const FName& FootIkCurveName, const FName& FootLockCurveName, const FTransform& ComponentTransformInverse, float DeltaTime) const;

	void ProcessFootLockTeleport(FFootState& FootState) const;

	void ProcessFootLockBaseChange(FFootState& FootState, const FTransform& ComponentTransformInverse) const;

	void UpdateFootLock(FFootState& FootState, const FName& FootLockCurveName, const FTransform& ComponentTransformInverse, float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const;

	void UpdateFootOffset(FFootState& FootState, float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const;

#pragma endregion


	/////////////////////////////////////////
	// Transitions State
#pragma region Transitions State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FTransitionsState TransitionsState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopBlendInDuration{ 0.1f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopBlendOutDuration{ 0.2f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions", Meta = (ClampMin = 0))
	FVector2f QuickStopPlayRate{ 1.75f, 3.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopStartTime{ 0.3f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions")
	TObjectPtr<UAnimSequenceBase> StandingTransitionLeftAnimation{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions")
	TObjectPtr<UAnimSequenceBase> StandingTransitionRightAnimation{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions")
	TObjectPtr<UAnimSequenceBase> CrouchingTransitionLeftAnimation{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions")
	TObjectPtr<UAnimSequenceBase> CrouchingTransitionRightAnimation{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float DynamicTransitionFootLockDistanceThreshold{ 8.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions", Meta = (ClampMin = 0, ForceUnits = "s"))
	float DynamicTransitionBlendDuration{ 0.2f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions", Meta = (ClampMin = 0, ForceUnits = "x"))
	float DynamicTransitionPlayRate{ 1.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions")
	TObjectPtr<UAnimSequenceBase> StandingDynamicTransitionLeftAnimation{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions")
	TObjectPtr<UAnimSequenceBase> StandingDynamicTransitionRightAnimation{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions")
	TObjectPtr<UAnimSequenceBase> CrouchingDynamicTransitionLeftAnimation{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Transitions")
	TObjectPtr<UAnimSequenceBase> CrouchingDynamicTransitionRightAnimation{ nullptr };

protected:
	void UpdateTransitions();

	void UpdateDynamicTransition();

	void PlayQueuedDynamicTransitionAnimation();

public:
	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance")
	void PlayQuickStopAnimation();

	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance")
	void PlayTransitionAnimation(UAnimSequenceBase* Animation, float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f, float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance")
	void PlayTransitionLeftAnimation(float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f, float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance")
	void PlayTransitionRightAnimation(float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f, float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "Human Anim Instance")
	void StopTransitionAndTurnInPlaceAnimations(float BlendOutDuration = 0.2f);

#pragma endregion
	

	/////////////////////////////////////////
	// Rotate In Place State
#pragma region Rotate In Place State
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FRotateInPlaceState RotateInPlaceState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Rotate In Place", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float ViewYawAngleThreshold{ 50.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Rotate In Place", Meta = (ClampMin = 0))
	FVector2f ReferenceViewYawSpeed{ 180.0, 460.0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Rotate In Place", Meta = (ClampMin = 0))
	FVector2f RotationInPlacePlayRate{ 1.15, 3.0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Rotate In Place", Meta = (ClampMin = 0, ClampMax = 180, EditCondition = "!bDisableFootLock", ForceUnits = "deg"))
	float FootLockBlockViewYawAngleThreshold{ 120.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Rotate In Place", Meta = (ClampMin = 0, EditCondition = "!bDisableFootLock", ForceUnits = "deg/s"))
	float FootLockBlockViewYawSpeedThreshold{ 620.0f };

protected:
	void UpdateRotateInPlace(float DeltaTime);

public:
	virtual bool IsRotateInPlaceAllowed();

#pragma endregion


	/////////////////////////////////////////
	// Control Rig Input
#pragma region Control Rig Input
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs|Hand IK")
	bool bUseHandIkBones{ true };

public:
	/**
	 * Get data to pass to ControlRig in BlueprintThreadSafe
	 */
	UFUNCTION(BlueprintPure, Category = "Human Anim Instance", Meta = (BlueprintThreadSafe, ReturnDisplayName = "Rig Input"))
	FControlRigInput GetControlRigInput() const;

#pragma endregion

};
