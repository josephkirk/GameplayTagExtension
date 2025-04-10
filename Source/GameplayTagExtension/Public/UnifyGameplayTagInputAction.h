#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "UnifyGameplayTagInputAction.generated.h"

class UInputAction;
class UObject;

USTRUCT(BlueprintType)
struct FUnifyGameplayTagInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

UCLASS(BlueprintType, Const)
class UUnifyGameplayTagInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UUnifyGameplayTagInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "UnifyGameplayTag")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	// UFUNCTION(BlueprintCallable, Category = "UnifyGameplayTag")
	// const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FUnifyGameplayTagInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	// TArray<FLyraInputAction> AbilityInputActions;
};