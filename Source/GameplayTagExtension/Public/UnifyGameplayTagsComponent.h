// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "UnifyGameplayTagsInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"
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

	/** Delegate signature for global message receive events */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceiveDelegate, const FUnifyGameplayTag&, Message);

	/** 
	 * Delegate that is broadcast when receiving a global message on the GameplayMessageTag channel 
	 * @param Message The gameplay tag message received
	 */
	UPROPERTY(BlueprintAssignable, Category = "GameplayTags")
	FOnMessageReceiveDelegate OnMessageReceive;

	/**
	 * Broadcasts the current tags of this component as a message on the GameplayMessageTag channel
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void BroadcastMessage();

	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void BroadcastMessageWithCustomData(FInstancedStruct Payload);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void SetGameplayMessageTag(FGameplayTag MessageTag);

	// Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End UActorComponent interface

protected:
	/** Container of gameplay tags for this component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayTags")
	FGameplayTagContainer GameplayTagContainer;

	/** The tag channel to listen to and broadcast on */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayTags|Message")
	FGameplayTag GameplayMessageTag;

	/** Filtered Source Object Tags To Handle Message */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags|Message")
	FGameplayTagContainer FilteredSourceTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags|Message")
	ETagMessageFilteredType MessageFilteredType;

	/** Optional payload for the event. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags|Message")
	FInstancedStruct MessageData;

	/** Handle for the registered message listener */
	FGameplayMessageListenerHandle MessageListenerHandle;

	/** 
	 * Internal handler for when a gameplay tag message is received 
	 * @param Channel The channel the message was sent on
	 * @param Message The message data received
	 */
	void HandleGameplayTagMessage(const FGameplayTag Channel, const FUnifyGameplayTag& Message);
};
