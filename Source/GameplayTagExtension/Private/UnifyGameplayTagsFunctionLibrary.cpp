// Copyright 2025 Nguyen Phi Hung. All Rights Reserved.

#include "UnifyGameplayTagsFunctionLibrary.h"

#include "EngineUtils.h"
#include "UnifyGameplayTagsComponent.h"
#include "Engine/Engine.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

UUnifyGameplayTagsComponent* UUnifyGameplayTagsFunctionLibrary::GetGameplayTagComponent(const AActor* Actor)
{
    return Cast<UUnifyGameplayTagsComponent>(Actor->GetComponentByClass(UUnifyGameplayTagsComponent::StaticClass()));
}

FGameplayTagContainer UUnifyGameplayTagsFunctionLibrary::GetGameplayTagContainer(const AActor* Actor, bool& bSuccess)
{
    bSuccess = GetGameplayTagComponent(Actor) != nullptr;
    return bSuccess ? GetGameplayTagComponent(Actor)->GetGameplayTagContainer() : FGameplayTagContainer();
}

TArray<AActor*> UUnifyGameplayTagsFunctionLibrary::FilterActorsWithGameplayTags(const TArray<AActor*>& ActorsToCheck, const FGameplayTagContainer TagsToCheck)
{
    TArray<AActor*> FilteredActors;
    for (AActor* Actor : ActorsToCheck)
    {
        UUnifyGameplayTagsComponent* Component = GetGameplayTagComponent(Actor);
        if (Component && Component->HasAnyGameplayTags(TagsToCheck))
        {
            FilteredActors.Add(Actor);
        }
    }
    return FilteredActors;
}

void UUnifyGameplayTagsFunctionLibrary::GetAllActorsWithGameplayTags(const UObject* WorldContextObject, const FGameplayTagContainer TagsToCheck, TArray<AActor*>& OutActors)
{
	QUICK_SCOPE_CYCLE_COUNTER(UGameplayStatics_GetAllActorsWithGameplayTag);
	OutActors.Reset();

	// We do nothing if no tag is provided, rather than giving ALL actors!
	if (TagsToCheck.IsEmpty())
	{
		return;
	}
	
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			UUnifyGameplayTagsComponent* Component = GetGameplayTagComponent(Actor);
            if (Component && Component->HasAnyGameplayTags(TagsToCheck))
            {
                OutActors.Add(Actor);
            }
		}
	}
}

void UUnifyGameplayTagsFunctionLibrary::GetAllActorsOfClassWithGameplayTags(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FGameplayTagContainer TagsToCheck, TArray<AActor*>& OutActors)
{
	QUICK_SCOPE_CYCLE_COUNTER(UGameplayStatics_GetAllActorsOFClassWithGameplayTag);
	OutActors.Reset();

	// We do nothing if no tag is provided, rather than giving ALL actors!
	if (TagsToCheck.IsEmpty())
	{
		return;
	}
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor->IsA(ActorClass))
			{
				UUnifyGameplayTagsComponent* Component = GetGameplayTagComponent(Actor);
				if (Component && Component->HasAnyGameplayTags(TagsToCheck))
				{
					OutActors.Add(Actor);
				}
			}
		}
	}
}

bool UUnifyGameplayTagsFunctionLibrary::IsActorHasGameplayTags(const UObject* WorldContextObject, const AActor* Actor, const FGameplayTagContainer TagsToCheck, EGameplayTagCheckType CheckType)
{
    UUnifyGameplayTagsComponent* Component = GetGameplayTagComponent(Actor);
    if (Component)
    {
        if (CheckType == EGameplayTagCheckType::Any)
        {
            return Component->HasAnyGameplayTags(TagsToCheck);
        }
        else if (CheckType == EGameplayTagCheckType::Exact)
        {
            return Component->HasAllGameplayTags(TagsToCheck);
        }
    }
    return false;
}

void UUnifyGameplayTagsFunctionLibrary::RunGameplayTagOperationOnActor(const UObject* WorldContextObject, const AActor* Actor, const FGameplayTagContainer Tags, ETagChangeType OperationType, FGameplayTagContainer& OutContainer, bool& bSuccess)
{
    UUnifyGameplayTagsComponent* Component = GetGameplayTagComponent(Actor);
    if (!Component)
    {
        bSuccess = false;
        return;
    }
    
    switch (OperationType)
    {
        case ETagChangeType::Set:
            Component->SetGameplayTagContainer(Tags);
            break;
        case ETagChangeType::Add:
            Component->AddGameplayTags(Tags);
            break;
        case ETagChangeType::Remove:
            Component->RemoveGameplayTags(Tags);
            break;
        case ETagChangeType::Clear:
            Component->ClearGameplayTags();
            break;
        default:
            break;
    }
    bSuccess = true; 
    OutContainer = Component->GetGameplayTagContainer();
}
