#include "UnifyGameplayTagInputAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnifyGameplayTagInputAction)

UUnifyGameplayTagInputConfig::UUnifyGameplayTagInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UUnifyGameplayTagInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FUnifyGameplayTagInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogGameplayTagExtension, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}