// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.

#include "UnifyGameplayTagsComponent.h"
#include "UnifyGameplayTagsFunctionLibrary.h"
#include "UnifyGameplayTagsSubsystem.h"
#include "GameplayTagExtension.h"
#include "UObject/UnrealType.h"
#include "Misc/AssertionMacros.h"

// Sets default values for this component's properties
UUnifyGameplayTagsComponent::UUnifyGameplayTagsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	// Initialize the event tags as invalid
	LastBoundMessageTag = FGameplayTag::EmptyTag;
	CurrentEventTag = FGameplayTag::EmptyTag;
}

void UUnifyGameplayTagsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Register this component with the UnifyGameplayTagsSubsystem
	if (UWorld* World = GetWorld())
	{
		if (UUnifyGameplayTagsSubsystem* Subsystem = World->GetSubsystem<UUnifyGameplayTagsSubsystem>())
		{
			Subsystem->RegisterComponent(this);
			// Update the event binding with the current tag
			UpdateEventBinding();
		}
	}
}

void UUnifyGameplayTagsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister this component from the UnifyGameplayTagsSubsystem
	if (UWorld* World = GetWorld())
	{
		if (UUnifyGameplayTagsSubsystem* Subsystem = World->GetSubsystem<UUnifyGameplayTagsSubsystem>())
		{
			// Unbind from the current gameplay tag event
			if (CurrentEventTag.IsValid())
			{
				Subsystem->UnbindAllGameplayTagEvents(this, CurrentEventTag);
			}
			
			Subsystem->UnregisterComponent(this);
		}
	}

	LastBoundMessageTag = FGameplayTag::EmptyTag;
	CurrentEventTag = FGameplayTag::EmptyTag;
	Super::EndPlay(EndPlayReason);
}

void UUnifyGameplayTagsComponent::SetGameplayMessageTag(const FGameplayTag& NewTag)
{
	if (GameplayMessageTag != NewTag)
	{
		GameplayMessageTag = NewTag;
		UpdateEventBinding(true); // Force rebind with the new tag
	}
}

void UUnifyGameplayTagsComponent::UpdateEventBinding(bool bForceRebind)
{
	// Only proceed if we have a valid world and subsystem
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	UUnifyGameplayTagsSubsystem* Subsystem = World->GetSubsystem<UUnifyGameplayTagsSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	// Check if we need to update the binding
	const bool bTagChanged = (CurrentEventTag != GameplayMessageTag);
	if (!bForceRebind && !bTagChanged)
	{
		return; // No need to update if the tag hasn't changed and we're not forcing a rebind
	}

	// Unbind from the previous tag if we have a valid tag
	if (CurrentEventTag.IsValid())
	{
		Subsystem->UnbindAllGameplayTagEvents(this, CurrentEventTag);
	}

	// Bind to the new tag if it's valid
	if (GameplayMessageTag.IsValid())
	{
		// Create a callback to our handler function
		FGameplayTagEventCallback Callback;
		Callback.BindUFunction(this, "HandleGameplayTagEvent");
		
		// Bind to the event
		Subsystem->BindGameplayTagEvent(this, GameplayMessageTag, Callback);
		
		// Update the current and last bound tags
		CurrentEventTag = GameplayMessageTag;
		LastBoundMessageTag = GameplayMessageTag;
	}
	else
	{
		// Clear the current event tag if we're not binding to a valid tag
		CurrentEventTag = FGameplayTag::EmptyTag;
	}
}

#if WITH_EDITOR
void UUnifyGameplayTagsComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Check if the GameplayMessageTag property was modified
	if (PropertyChangedEvent.Property && 
		(PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UUnifyGameplayTagsComponent, GameplayMessageTag)))
	{
		// Only update if we're not in the game (PIE or standalone)
		if (GIsEditor && !GIsPlayInEditorWorld && !IsRunningGame())
		{
			// Force an update of the binding
			UpdateEventBinding(true);
		}
	}
}
#endif

void UUnifyGameplayTagsComponent::HandleGameplayTagEvent(UObject* Dispatcher, FGameplayTageMessageData& Data)
{
	// Broadcast the event to any bound delegates
	OnGameplayTagEventReceived.Broadcast(Dispatcher, Data);
}

FGameplayTagContainer UUnifyGameplayTagsComponent::GetGameplayTagContainer_Implementation() const
{
	return GameplayTagContainer;
}

void UUnifyGameplayTagsComponent::SetGameplayTagContainer_Implementation(const FGameplayTagContainer& NewTagContainer)
{
	GameplayTagContainer = NewTagContainer;
	OnGameplayTagContainerChanged.Broadcast(GameplayTagContainer, ETagChangeType::Set);
}

void UUnifyGameplayTagsComponent::AddGameplayTag_Implementation(const FGameplayTag& TagToAdd)
{
	if (!TagToAdd.IsValid())
	{
		return;
	}
	
	GameplayTagContainer.AddTag(TagToAdd);
	OnGameplayTagContainerChanged.Broadcast(FGameplayTagContainer(GameplayTagContainer), ETagChangeType::Add);
}

void UUnifyGameplayTagsComponent::AddGameplayTags_Implementation(const FGameplayTagContainer& TagsToAdd)
{
	if (TagsToAdd.IsEmpty())
	{
		return;
	}
	
	GameplayTagContainer.AppendTags(TagsToAdd);
	OnGameplayTagContainerChanged.Broadcast(FGameplayTagContainer(GameplayTagContainer), ETagChangeType::Add);
}

void UUnifyGameplayTagsComponent::RemoveGameplayTag_Implementation(const FGameplayTag& TagToRemove)
{
	if (!TagToRemove.IsValid() || !GameplayTagContainer.HasTag(TagToRemove))
	{
		return;
	}
	
	GameplayTagContainer.RemoveTag(TagToRemove);
	OnGameplayTagContainerChanged.Broadcast(FGameplayTagContainer(GameplayTagContainer), ETagChangeType::Remove);
}

void UUnifyGameplayTagsComponent::RemoveGameplayTags_Implementation(const FGameplayTagContainer& TagsToRemove)
{
	if (TagsToRemove.IsEmpty())
	{
		return;
	}
	
	bool bAnyRemoved = false;
	for (const FGameplayTag& Tag : TagsToRemove)
	{
		if (GameplayTagContainer.HasTag(Tag))
		{
			GameplayTagContainer.RemoveTag(Tag);
			bAnyRemoved = true;
		}
	}
	
	if (bAnyRemoved)
	{
		OnGameplayTagContainerChanged.Broadcast(FGameplayTagContainer(GameplayTagContainer), ETagChangeType::Remove);
	}
}

bool UUnifyGameplayTagsComponent::HasGameplayTag_Implementation(const FGameplayTag& TagToCheck) const
{
	return GameplayTagContainer.HasTag(TagToCheck);
}

bool UUnifyGameplayTagsComponent::HasAllGameplayTags_Implementation(const FGameplayTagContainer& TagsToCheck) const
{
	return GameplayTagContainer.HasAll(TagsToCheck);
}

bool UUnifyGameplayTagsComponent::HasAnyGameplayTags_Implementation(const FGameplayTagContainer& TagsToCheck) const
{
	return GameplayTagContainer.HasAny(TagsToCheck);
}

void UUnifyGameplayTagsComponent::ClearGameplayTags_Implementation()
{
	if (!GameplayTagContainer.IsEmpty())
	{
		FGameplayTagContainer OldContainer = FGameplayTagContainer(GameplayTagContainer);
		GameplayTagContainer.Reset();
		OnGameplayTagContainerChanged.Broadcast(OldContainer, ETagChangeType::Clear);
	}
}
