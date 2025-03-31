// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnifyGameplayTagsComponent.h"

// Sets default values for this component's properties
UUnifyGameplayTagsComponent::UUnifyGameplayTagsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = false;
}

FGameplayTagContainer UUnifyGameplayTagsComponent::GetGameplayTagContainer_Implementation() const
{
	return GameplayTagContainer;
}

void UUnifyGameplayTagsComponent::SetGameplayTagContainer_Implementation(const FGameplayTagContainer& NewTagContainer)
{
	GameplayTagContainer = NewTagContainer;
	OnGameplayTagContainerChanged.Broadcast();
}

void UUnifyGameplayTagsComponent::AddGameplayTag_Implementation(const FGameplayTag& TagToAdd)
{
	if (!TagToAdd.IsValid())
	{
		return;
	}
	
	GameplayTagContainer.AddTag(TagToAdd);
	OnGameplayTagContainerChanged.Broadcast();
}

void UUnifyGameplayTagsComponent::AddGameplayTags_Implementation(const FGameplayTagContainer& TagsToAdd)
{
	if (TagsToAdd.IsEmpty())
	{
		return;
	}
	
	GameplayTagContainer.AppendTags(TagsToAdd);
	OnGameplayTagContainerChanged.Broadcast();
}

void UUnifyGameplayTagsComponent::RemoveGameplayTag_Implementation(const FGameplayTag& TagToRemove)
{
	if (!TagToRemove.IsValid() || !GameplayTagContainer.HasTag(TagToRemove))
	{
		return;
	}
	
	GameplayTagContainer.RemoveTag(TagToRemove);
	OnGameplayTagContainerChanged.Broadcast();
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
		OnGameplayTagContainerChanged.Broadcast();
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
		GameplayTagContainer.Reset();
		OnGameplayTagContainerChanged.Broadcast();
	}
}
