// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "UnifyGameplayTagsInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "UnifyGameplayTagsSubsystem.generated.h"

class UUnifyGameplayTagsComponent;
struct FGameplayTageMessageData;
/**
 * A delegate linked to a single event. Used to bind/unbind (subscribe/unsubscribe) an event to a global multicast event.
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FGameplayTagEventCallback, UObject*, Listener, FGameplayTageMessageData, Data);

/**
 * A multicast delegate linked to many delegates. Used to call (publish) to every linked delegate associated to this multicast.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameplayTagEventMulticast, UObject*, Dispatcher, FGameplayTageMessageData, Data);

/**
 * World subsystem for managing UnifyGameplayTags components and global gameplay tag events
 * Provides a central registry for UnifyGameplayTagsComponents in the world and a global event system using GameplayTags
 */
UCLASS()
class GAMEPLAYTAGEXTENSION_API UUnifyGameplayTagsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UUnifyGameplayTagsSubsystem();

	// Begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem interface

#pragma region Component Management
	/**
	 * Register a component with the subsystem
	 * @param Component The component to register
	 */
	void RegisterComponent(UUnifyGameplayTagsComponent* Component);

	/**
	 * Unregister a component from the subsystem
	 * @param Component The component to unregister
	 */
	void UnregisterComponent(UUnifyGameplayTagsComponent* Component);

	/**
	 * Get all registered components
	 * @return Array of registered UnifyGameplayTagsComponents
	 */
	TArray<UUnifyGameplayTagsComponent*> GetRegisteredComponents() const { return RegisteredComponents; }

	/**
	 * Get all components with a specific gameplay tag
	 * @param Tag The gameplay tag to check for
	 * @return Array of components that have the specified tag
	 */
	TArray<UUnifyGameplayTagsComponent*> GetComponentsWithTag(const FGameplayTag& Tag) const;

	/**
	 * Get all components with any of the specified gameplay tags
	 * @param Tags The gameplay tags to check for
	 * @return Array of components that have any of the specified tags
	 */
	TArray<UUnifyGameplayTagsComponent*> GetComponentsWithAnyTags(const FGameplayTagContainer& Tags) const;

	/**
	 * Get all components with all of the specified gameplay tags
	 * @param Tags The gameplay tags to check for
	 * @return Array of components that have all of the specified tags
	 */
	TArray<UUnifyGameplayTagsComponent*> GetComponentsWithAllTags(const FGameplayTagContainer& Tags) const;
#pragma endregion

#pragma region Event System
	/**
	 * Bind a listener Object to a Gameplay Tag Event
	 * @param Listener The object that will listen to the event (usually 'this')
	 * @param EventTag The gameplay tag that identifies the event
	 * @param Callback The function to call when the event is triggered
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags|Events", meta = (DefaultToSelf = "Listener", HidePin = "Listener"))
	void BindGameplayTagEvent(UObject* Listener, const FGameplayTag& EventTag, const FGameplayTagEventCallback& Callback);

	/**
	 * Get all objects listening to a specific gameplay tag event
	 * @param EventTag The gameplay tag that identifies the event
	 * @return Array of objects that are listening to the event
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags|Events")
	TArray<UObject*> GetGameplayTagEventListeners(const FGameplayTag& EventTag) const;

	/**
	 * Unbind a specific callback from a gameplay tag event
	 * @param Event The callback to unbind
	 * @param EventTag The gameplay tag that identifies the event
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags|Events")
	void UnbindGameplayTagEvent(const FGameplayTagEventCallback& Event, const FGameplayTag& EventTag);

	/**
	 * Unbind all callbacks from a specific object for a gameplay tag event
	 * @param Listener The object whose callbacks should be unbound
	 * @param EventTag The gameplay tag that identifies the event
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags|Events")
	void UnbindAllGameplayTagEvents(UObject* Listener, const FGameplayTag& EventTag);

	/**
	 * Trigger a gameplay tag event
	 * @param Dispatcher The object that is dispatching the event (usually 'this')
	 * @param EventTag The gameplay tag that identifies the event
	 * @param DataObject Optional data to pass to the event listeners
	 */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags|Events", meta = (DefaultToSelf = "Dispatcher", HidePin = "Dispatcher"))
	void TriggerGameplayTagEvent(UObject* Dispatcher, const FGameplayTag& EventTag, FGameplayTageMessageData Data);
#pragma endregion

private:
	/** Array of registered components */
	TArray<UUnifyGameplayTagsComponent*> RegisteredComponents;

	/** Map that stores the Gameplay Tag Events */
	UPROPERTY()
	TMap<FGameplayTag, FGameplayTagEventMulticast> GameplayTagEventsMap;
};
