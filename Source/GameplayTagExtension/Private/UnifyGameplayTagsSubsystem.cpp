// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.

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
			TArray<FGameplayTagEventListener>& ListenersArray = EventPair.Value.Listeners;
			ListenersArray.RemoveAll([Component](const FGameplayTagEventListener& ListenerEntry)
			{
				return ListenerEntry.Callback.IsBoundToObject(Component);
			});
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

void UUnifyGameplayTagsSubsystem::BindGameplayTagEvent(UObject* Listener, const FGameplayTag& EventTag, const FGameplayTagEventCallback& Callback, const FGameplayTagContainer& ListenerFilterTags)
{
	if (Listener && EventTag.IsValid() && Callback.IsBound())
	{
		TArray<FGameplayTagEventListener>& ListenersArray = GameplayTagEventsMap.FindOrAdd(EventTag).Listeners;
		// Add a new listener entry, ensuring no duplicates for the same callback
		ListenersArray.AddUnique(FGameplayTagEventListener(Callback, ListenerFilterTags));
	}
}

TArray<UObject*> UUnifyGameplayTagsSubsystem::GetGameplayTagEventListeners(const FGameplayTag& EventTag) const
{
	TArray<UObject*> Result;
	if (const FGameplayTagEventListenerArrayWrapper* WrapperPtr = GameplayTagEventsMap.Find(EventTag))
	{
		for (const FGameplayTagEventListener& ListenerEntry : WrapperPtr->Listeners)
		{
			if (ListenerEntry.Callback.IsBound())
			{
				Result.Add(const_cast<UObject*>(ListenerEntry.Callback.GetUObject()));
			}
		}
	}
	return Result;
}

void UUnifyGameplayTagsSubsystem::UnbindGameplayTagEvent(const FGameplayTagEventCallback& Event, const FGameplayTag& EventTag)
{
	if (FGameplayTagEventListenerArrayWrapper* Wrapper = GameplayTagEventsMap.Find(EventTag))
	{
		// Create a temporary FGameplayTagEventListener to use the operator== for removal
		// The ListenerFilterTags part of FGameplayTagEventListener doesn't matter for this comparison
		Wrapper->Listeners.Remove(FGameplayTagEventListener(Event, FGameplayTagContainer()));
	}
}

void UUnifyGameplayTagsSubsystem::UnbindAllGameplayTagEvents(UObject* Listener, const FGameplayTag& EventTag)
{
	if (FGameplayTagEventListenerArrayWrapper* Wrapper = GameplayTagEventsMap.Find(EventTag))
	{
		Wrapper->Listeners.RemoveAll([Listener](const FGameplayTagEventListener& ListenerEntry)
		{
			return ListenerEntry.Callback.IsBoundToObject(Listener);
		});
	}
}

void UUnifyGameplayTagsSubsystem::TriggerGameplayTagEvent(UObject* Dispatcher, const FGameplayTag EventTag, FGameplayTagMessageData Data, const FGameplayTagContainer EventPayloadTags)
{
	if (EventTag.IsValid())
	{
		if (FGameplayTagEventListenerArrayWrapper* WrapperPtr = GameplayTagEventsMap.Find(EventTag))
		{
			// Create a copy of the listeners array to avoid issues if a callback modifies the original array (e.g., unbinds itself)
			TArray<FGameplayTagEventListener> ListenersCopy = WrapperPtr->Listeners;
			for (const FGameplayTagEventListener& ListenerEntry : ListenersCopy)
			{
				if (ListenerEntry.Callback.IsBound())
				{
					// Check if the listener's filter tags are met by the event's payload tags
					// An empty ListenerFilterTags means it accepts all events for this EventTag.
					// Otherwise, EventPayloadTags must contain all tags in ListenerFilterTags.
					bool bFilterPassed = ListenerEntry.ListenerFilterTags.IsEmpty() || EventPayloadTags.HasAll(ListenerEntry.ListenerFilterTags);

					if (bFilterPassed)
					{
						ListenerEntry.Callback.ExecuteIfBound(Dispatcher, Data);
					}
				}
			}
		}
	}
}
