// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnifyGameplayTagsComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

// Sets default values for this component's properties
UUnifyGameplayTagsComponent::UUnifyGameplayTagsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = false;
}

void UUnifyGameplayTagsComponent::BeginPlay()
{
	Super::BeginPlay();
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	// Register for gameplay tag messages if we have a valid message tag
	if (GameplayMessageTag.IsValid())
	{
		MessageListenerHandle = MessageSystem.RegisterListener<FUnifyGameplayTag>(
			GameplayMessageTag,
			this,
			&UUnifyGameplayTagsComponent::HandleGameplayTagMessage);
	}
}

void UUnifyGameplayTagsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister the message listener
	if (MessageListenerHandle.IsValid())
	{
		MessageListenerHandle.Unregister();
	}

	Super::EndPlay(EndPlayReason);
}

void UUnifyGameplayTagsComponent::HandleGameplayTagMessage(FGameplayTag Channel, const FUnifyGameplayTag& Message)
{
	// Broadcast the entire message struct
	OnMessageReceive.Broadcast(Message);
}

void UUnifyGameplayTagsComponent::BroadcastMessage()
{
	// Only broadcast if we have a valid message tag
	if (!GameplayMessageTag.IsValid())
	{
		return;
	}
	
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
		// Create and populate the message struct
		FUnifyGameplayTag Message;
		Message.SourceObject = Cast<UObject>(GetOwner());
		Message.Tags = GameplayTagContainer;

		// Broadcast the message
		MessageSystem.BroadcastMessage(GameplayMessageTag, Message);
	}
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
