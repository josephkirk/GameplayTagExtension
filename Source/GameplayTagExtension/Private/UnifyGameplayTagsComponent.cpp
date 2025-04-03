// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnifyGameplayTagsComponent.h"
#include "UnifyGameplayTagsFunctionLibrary.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UnifyGameplayTagsSubsystem.h"

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
	
	// Register this component with the UnifyGameplayTagsSubsystem
	if (UWorld* World = GetWorld())
	{
		if (UUnifyGameplayTagsSubsystem* Subsystem = World->GetSubsystem<UUnifyGameplayTagsSubsystem>())
		{
			Subsystem->RegisterComponent(this);
		}
	}
}

void UUnifyGameplayTagsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister the message listener
	if (MessageListenerHandle.IsValid())
	{
		MessageListenerHandle.Unregister();
	}
	
	// Unregister this component from the UnifyGameplayTagsSubsystem
	if (UWorld* World = GetWorld())
	{
		if (UUnifyGameplayTagsSubsystem* Subsystem = World->GetSubsystem<UUnifyGameplayTagsSubsystem>())
		{
			Subsystem->UnregisterComponent(this);
		}
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
	BroadcastMessageWithCustomData(MessageData);
}

void UUnifyGameplayTagsComponent::BroadcastMessageWithCustomData(FInstancedStruct Payload)
{
	// Only broadcast if we have a valid message tag
	if (!GameplayMessageTag.IsValid())
	{
		return;
	}
	
	FUnifyGameplayTag Message;
	Message.SourceObject = Cast<UObject>(GetOwner());
	Message.Tags = GameplayTagContainer;
	Message.Payload = Payload;
	// Broadcast the message
	UUnifyGameplayTagsFunctionLibrary::BroadcastGameplayTagMessage(this, GameplayMessageTag, FInstancedStruct::Make(Message));
}

void UUnifyGameplayTagsComponent::SetGameplayMessageTag(FGameplayTag MessageTag)
{
	GameplayMessageTag = MessageTag;
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
