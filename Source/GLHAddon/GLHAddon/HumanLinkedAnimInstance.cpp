#include "HumanLinkedAnimInstance.h"

#include "HumanAnimInstance.h"
#include "HumanAnimInstanceProxy.h"

#include "CharacterMeshAccessorInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HumanLinkedAnimInstance)


UHumanLinkedAnimInstance::UHumanLinkedAnimInstance()
{
	RootMotionMode = ERootMotionMode::IgnoreRootMotion;
	bUseMainInstanceMontageEvaluationData = true;
}


void UHumanLinkedAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ALocomotionCharacter>(GetOwningActor());
	
#if WITH_EDITOR
	if (!GetWorld()->IsGameWorld())
	{
		// Use default object for editor preview.

		if (!Parent.IsValid())
		{
			Parent = GetMutableDefault<UHumanAnimInstance>();
		}

		if (!IsValid(Character))
		{
			Character = GetMutableDefault<ALocomotionCharacter>();
		}
		return;
	}
#endif

	auto* Mesh{ ICharacterMeshAccessorInterface::Execute_GetMainMesh(this) };
	
	Parent = Mesh ? Cast<UHumanAnimInstance>(Mesh->GetAnimInstance()) : nullptr;
}

void UHumanLinkedAnimInstance::NativeBeginPlay()
{
	ensureMsgf(Parent.IsValid(), TEXT("Parent is invalid. Parent must inherit from UCharacterAnimInstance."));

	Super::NativeBeginPlay();
}

FAnimInstanceProxy* UHumanLinkedAnimInstance::CreateAnimInstanceProxy()
{
	return new FHumanAnimInstanceProxy(this);
}

UHumanAnimInstance* UHumanLinkedAnimInstance::GetParentUnsafe() const
{
	return Parent.Get();
}

void UHumanLinkedAnimInstance::ReinitializeLook()
{
	if (Parent.IsValid())
	{
		Parent->ReinitializeLook();
	}
}

void UHumanLinkedAnimInstance::RefreshLook()
{
	if (Parent.IsValid())
	{
		Parent->UpdateLook();
	}
}

void UHumanLinkedAnimInstance::SetHipsDirection(const EHipsDirection NewHipsDirection)
{
	if (Parent.IsValid())
	{
		Parent->SetHipsDirection(NewHipsDirection);
	}
}

void UHumanLinkedAnimInstance::ActivatePivot()
{
	if (Parent.IsValid())
	{
		Parent->ActivatePivot();
	}
}

void UHumanLinkedAnimInstance::ResetJumped()
{
	if (Parent.IsValid())
	{
		Parent->ResetJumped();
	}
}
