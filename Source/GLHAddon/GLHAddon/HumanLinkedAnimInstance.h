#pragma once

#include "Animation/AnimInstance.h"

#include "HumanLinkedAnimInstance.generated.h"

class ALocomotionCharacter;
class UHumanAnimInstance;
enum class EHipsDirection : uint8;

/**
 * Sub animations without main state processing for Linked Anim Layers applied to Character
 * 
 * Tips:
 *  Can be used as AnimInstance for FPP in addition to Linked Anim Layers
 */
UCLASS()
class GLHADDON_API UHumanLinkedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UHumanLinkedAnimInstance();

protected:
	//
	// AnimInstance that handles the main processing of the Character.
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Refarence", Transient)
	TWeakObjectPtr<UHumanAnimInstance> Parent;

	//
	// The Character that owns this AnimInstance.
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Refarence", Transient)
	TObjectPtr<ALocomotionCharacter> Character;

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;

protected:
	//
	// Use PropertyAccess to read the main AnimInstace variable
	// 
	// ========== WARNNING =========
	// Since this function is guaranteed to be called before parallel animation evaluation
	// It is safe to read variables that are modified only inside Parent's UCharacterAnimationInstance::NativeUpdateAnimation()
	// 
	// If you don't know what you are doing, access the variable through the Parent variable
	//
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = " Linked Animation Instance", Meta = (BlueprintProtected, BlueprintThreadSafe, ReturnDisplayName = "Parent"))
	UHumanAnimInstance* GetParentUnsafe() const;

	UFUNCTION(BlueprintCallable, Category = " Linked Animation Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void ReinitializeLook();

	UFUNCTION(BlueprintCallable, Category = " Linked Animation Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void RefreshLook();

	UFUNCTION(BlueprintCallable, Category = " Linked Animation Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void SetHipsDirection(EHipsDirection NewHipsDirection);

	UFUNCTION(BlueprintCallable, Category = " Linked Animation Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void ActivatePivot();

	UFUNCTION(BlueprintCallable, Category = " Linked Animation Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	void ResetJumped();

};
