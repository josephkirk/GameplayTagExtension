// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnifyGameplayTagsSubsystem.h"
#include "UnifyGameplayTagsComponent.h"

UUnifyGameplayTagsSubsystem::UUnifyGameplayTagsSubsystem()
{
}

void UUnifyGameplayTagsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUnifyGameplayTagsSubsystem::Deinitialize()
{
	// Clear the registered components array
	RegisteredComponents.Empty();
	
	// Clear all event bindings
	GameplayTagEventsMap.Empty();
	
	Super::Deinitialize();
}

void UUnifyGameplayTagsSubsystem::RegisterComponent(UUnifyGameplayTagsComponent* Component)
{
	if (Component && !RegisteredComponents.Contains(Component))
	{
		RegisteredComponents.Add(Component);
	}
}

void UUnifyGameplayTagsSubsystem::UnregisterComponent(UUnifyGameplayTagsComponent* Component)
{
	if (Component)
	{
		// Remove from registered components
		RegisteredComponents.Remove(Component);
		
		// Remove all event bindings for this component
		for (auto& EventPair : GameplayTagEventsMap)
		{
			EventPair.Value.RemoveAll(Component);
		}
	}
}

TArray<UUnifyGameplayTagsComponent*> UUnifyGameplayTagsSubsystem::GetComponentsWithTag(const FGameplayTag& Tag) const
{
	TArray<UUnifyGameplayTagsComponent*> Result;
	for (UUnifyGameplayTagsComponent* Component : RegisteredComponents)
	{
		if (Component && Component->HasGameplayTag(Tag))
		{
			Result.Add(Component);
		}
	}
	return Result;
}

TArray<UUnifyGameplayTagsComponent*> UUnifyGameplayTagsSubsystem::GetComponentsWithAnyTags(const FGameplayTagContainer& Tags) const
{
	TArray<UUnifyGameplayTagsComponent*> Result;
	for (UUnifyGameplayTagsComponent* Component : RegisteredComponents)
	{
		if (Component && Component->HasAnyGameplayTags(Tags))
		{
			Result.Add(Component);
		}
	}
	return Result;
}

TArray<UUnifyGameplayTagsComponent*> UUnifyGameplayTagsSubsystem::GetComponentsWithAllTags(const FGameplayTagContainer& Tags) const
{
	TArray<UUnifyGameplayTagsComponent*> Result;
	for (UUnifyGameplayTagsComponent* Component : RegisteredComponents)
	{
		if (Component && Component->HasAllGameplayTags(Tags))
		{
			Result.Add(Component);
		}
	}
	return Result;
}

void UUnifyGameplayTagsSubsystem::BindGameplayTagEvent(UObject* Listener, const FGameplayTag& EventTag, const FGameplayTagEventCallback& Callback)
{
	if (Listener && EventTag.IsValid())
	{
		// Find or add the multicast delegate for this event tag
		FGameplayTagEventMulticast& MulticastDelegate = GameplayTagEventsMap.FindOrAdd(EventTag);
		
		// Add the callback to the multicast delegate
		MulticastDelegate.Add(Callback);
	}
}

TArray<UObject*> UUnifyGameplayTagsSubsystem::GetGameplayTagEventListeners(const FGameplayTag& EventTag) const
{
	if (const FGameplayTagEventMulticast* MulticastDelegate = GameplayTagEventsMap.Find(EventTag))
	{
		return MulticastDelegate->GetAllObjects();
	}
	return TArray<UObject*>();
}

void UUnifyGameplayTagsSubsystem::UnbindGameplayTagEvent(const FGameplayTagEventCallback& Event, const FGameplayTag& EventTag)
{
	if (FGameplayTagEventMulticast* MulticastDelegate = GameplayTagEventsMap.Find(EventTag))
	{
		MulticastDelegate->Remove(Event);
	}
}

void UUnifyGameplayTagsSubsystem::UnbindAllGameplayTagEvents(UObject* Listener, const FGameplayTag& EventTag)
{
	if (FGameplayTagEventMulticast* MulticastDelegate = GameplayTagEventsMap.Find(EventTag))
	{
		MulticastDelegate->RemoveAll(Listener);
	}
}

void UUnifyGameplayTagsSubsystem::TriggerGameplayTagEvent(UObject* Dispatcher, const FGameplayTag& EventTag, UObject* DataObject)
{
	if (EventTag.IsValid())
	{
		if (FGameplayTagEventMulticast* MulticastDelegate = GameplayTagEventsMap.Find(EventTag))
		{
			// Broadcast the event to all bound delegates
			MulticastDelegate->Broadcast(Dispatcher, DataObject);
		}
	}
}
