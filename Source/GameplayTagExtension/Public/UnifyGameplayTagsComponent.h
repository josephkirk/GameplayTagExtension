// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "UnifyGameplayTagsInterface.h"
#include "UnifyGameplayTagsComponent.generated.h"

/**
 * Component that implements the UnifyGameplayTagsInterface
 * Add this component to any actor that needs to work with gameplay tags
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType)
class GAMEPLAYTAGEXTENSION_API UUnifyGameplayTagsComponent : public UActorComponent, public IUnifyGameplayTagsInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnifyGameplayTagsComponent();

	// Begin IUnifyGameplayTagsInterface
	virtual FGameplayTagContainer GetGameplayTagContainer_Implementation() const override;
	virtual void SetGameplayTagContainer_Implementation(const FGameplayTagContainer& NewTagContainer) override;
	virtual void AddGameplayTag_Implementation(const FGameplayTag& TagToAdd) override;
	virtual void AddGameplayTags_Implementation(const FGameplayTagContainer& TagsToAdd) override;
	virtual void RemoveGameplayTag_Implementation(const FGameplayTag& TagToRemove) override;
	virtual void RemoveGameplayTags_Implementation(const FGameplayTagContainer& TagsToRemove) override;
	virtual bool HasGameplayTag_Implementation(const FGameplayTag& TagToCheck) const override;
	virtual bool HasAllGameplayTags_Implementation(const FGameplayTagContainer& TagsToCheck) const override;
	virtual bool HasAnyGameplayTags_Implementation(const FGameplayTagContainer& TagsToCheck) const override;
	virtual void ClearGameplayTags_Implementation() override;
	// End IUnifyGameplayTagsInterface

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTagContainerChanged);

	/** Delegate that is broadcast whenever the tag container is changed */
	UPROPERTY(BlueprintAssignable, Category = "GameplayTags")
	FOnTagContainerChanged OnGameplayTagContainerChanged;

protected:
	/** Container of gameplay tags for this component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayTags")
	FGameplayTagContainer GameplayTagContainer;
};
