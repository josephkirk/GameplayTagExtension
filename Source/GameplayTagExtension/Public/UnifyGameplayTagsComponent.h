// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "UnifyGameplayTagsInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UnifyGameplayTagsComponent.generated.h"

/**
 * Message struct for UnifyGameplayTags communication
 */
USTRUCT(BlueprintType)
struct GAMEPLAYTAGEXTENSION_API FUnifyGameplayTag
{
	GENERATED_BODY()

	/** The object that broadcast the message */
	UPROPERTY(BlueprintReadWrite, Category = "GameplayTags")
	UObject* SourceObject = nullptr;

	/** The object that broadcast the message */
	UPROPERTY(BlueprintReadWrite, Category = "GameplayTags")
	UObject* DataObject = nullptr;

	/** Container of gameplay tags being communicated */
	UPROPERTY(BlueprintReadWrite, Category = "GameplayTags")
	FGameplayTagContainer Tags;
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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTagContainerChanged);

	/** Delegate that is broadcast whenever the tag container is changed */
	UPROPERTY(BlueprintAssignable, Category = "GameplayTags")
	FOnTagContainerChanged OnGameplayTagContainerChanged;

	/** Delegate signature for message receive events */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceiveDelegate, const FUnifyGameplayTag&, Message);

	/** 
	 * Delegate that is broadcast when receiving a message on the GameplayMessageTag channel 
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
	void BroadcastMessageWithCustomData(UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void SetGameplayMessageTag(FGameplayTag MessageTag);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void SetGameplayMessageData(UObject* Data);

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

	UPROPERTY(BlueprintReadWrite, Category = "GameplayTags|Message")
	UObject* MessageData;

	/** Handle for the registered message listener */
	FGameplayMessageListenerHandle MessageListenerHandle;

	/** 
	 * Internal handler for when a gameplay tag message is received 
	 * @param Channel The channel the message was sent on
	 * @param Message The message data received
	 */
	void HandleGameplayTagMessage(FGameplayTag Channel, const FUnifyGameplayTag& Message);
};
