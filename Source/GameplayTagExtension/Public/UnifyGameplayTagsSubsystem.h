// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Subsystems/WorldSubsystem.h"
#include "UnifyGameplayTagsSubsystem.generated.h"

class UUnifyGameplayTagsComponent;

/**
 * World subsystem for managing UnifyGameplayTags components
 * Provides a central registry for UnifyGameplayTagsComponents in the world
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

private:
	/** Array of registered components */
	TArray<UUnifyGameplayTagsComponent*> RegisteredComponents;
};
