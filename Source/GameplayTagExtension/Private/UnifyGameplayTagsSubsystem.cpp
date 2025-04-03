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
		RegisteredComponents.Remove(Component);
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
