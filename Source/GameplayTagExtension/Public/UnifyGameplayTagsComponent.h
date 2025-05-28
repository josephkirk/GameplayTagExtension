// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "UnifyGameplayTagsInterface.h"
#include "UnifyGameplayTagsSubsystem.h"
#include "UnifyGameplayTagsComponent.generated.h"


UENUM()
enum class ETagChangeType : uint8
{
	Set,
	Add,
	Remove,
	Clear
};

UENUM()
enum class ETagMessageFilteredType : uint8
{
	Include,
	Exclude,
	None
};
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

	/** 
	 * Delegate signature for tag container change events
	 * @param TagContainer The new tag container
	 * @param ChangeType The type of change that occurred
	 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagContainerChanged, const FGameplayTagContainer&, TagContainer, ETagChangeType, ChangeType);

	/** Delegate that is broadcast whenever the tag container is changed */
	UPROPERTY(BlueprintAssignable, Category = "GameplayTags")
	FOnTagContainerChanged OnGameplayTagContainerChanged;

	/** Delegate that is broadcast when receiving a gameplay tag event */
	UPROPERTY(BlueprintAssignable, Category = "GameplayTags|Events")
	FGameplayTagEventMulticast OnGameplayTagEventReceived;

	// Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End UActorComponent interface

protected:
	/** Container of gameplay tags for this component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayTags")
	FGameplayTagContainer GameplayTagContainer;

	/** The tag channel to listen to and broadcast on */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayTags|Message", meta = (DisplayName = "Gameplay Message Tag"))
	FGameplayTag GameplayMessageTag;

	/** 
	 * Sets the gameplay message tag and updates the event binding
	 * @param NewTag The new gameplay message tag to listen for
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags|Message")
	void SetGameplayMessageTag(const FGameplayTag& NewTag);

#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface
#endif

	/** 
	 * Internal handler for when a gameplay tag event is received 
	 * @param Dispatcher The object that dispatched the event
	 * @param DataObject Optional data object passed with the event
	 */
	UFUNCTION()
	void HandleGameplayTagEvent(UObject* Dispatcher, UObject* DataObject);

	/** 
	 * Updates the event binding to the current GameplayMessageTag 
	 * @param bForceRebind If true, will rebind even if the tag hasn't changed
	 */
	void UpdateEventBinding(bool bForceRebind = false);

	/** The last tag we were bound to */
	FGameplayTag LastBoundMessageTag;
	
	/** The current event tag we're bound to */
	FGameplayTag CurrentEventTag;
};
