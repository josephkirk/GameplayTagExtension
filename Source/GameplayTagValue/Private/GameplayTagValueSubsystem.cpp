// Copyright Windsurf Engineering, 2025. All Rights Reserved.

#include "GameplayTagValueSubsystem.h"
#include "GameplayTagValue.h" // For LogGameplayTagValue
#include "FMemoryTagValueRepository.h" // For the default internal repository
#include "UObject/UObjectIterator.h"   // For finding the subsystem instance
#include "Engine/GameInstance.h"       // For GetGameInstance()

// It's good practice to define a log category for your module
// DECLARE_LOG_CATEGORY_EXTERN(LogGameplayTagValue, Log, All); // This would go in a shared header like GameplayTagValue.h
// DEFINE_LOG_CATEGORY(LogGameplayTagValue); // This would go in a .cpp, perhaps GameplayTagValueModule.cpp

void UGameplayTagValueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Create and register the default internal repository
    DefaultRepositoryInternal = MakeShared<FMemoryTagValueRepository>(DefaultInternalRepositoryName, DefaultInternalRepositoryPriority);
    
    // Manually create a TScriptInterface for the non-UObject repository
    // This is a bit more involved as TScriptInterface is typically for UObjects.
    // We'll store it internally but won't add it to the 'Repositories' TArray in the same way
    // as UObject-based repositories to avoid lifetime management issues with TScriptInterface
    // if it were to point to a raw pointer that gets deleted.
    // For simplicity in this iteration, the DefaultRepositoryInternal will be checked explicitly.
    // A more robust solution might involve a wrapper UObject for non-UObject repositories if they need to be in the TArray.

    UE_LOG(LogGameplayTagValue, Log, TEXT("GameplayTagValueSubsystem Initialized. Default Memory Repository: %s"), *DefaultRepositoryInternal->GetRepositoryName().ToString());
}

void UGameplayTagValueSubsystem::Deinitialize()
{
    DefaultRepositoryInternal.Reset();
    Repositories.Empty();
    UE_LOG(LogGameplayTagValue, Log, TEXT("GameplayTagValueSubsystem Deinitialized."));
    Super::Deinitialize();
}

UGameplayTagValueSubsystem* UGameplayTagValueSubsystem::Get(const UObject* WorldContextObject)
{
    if (WorldContextObject)
    {
        if (UGameInstance* GameInstance = WorldContextObject->GetWorld()->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UGameplayTagValueSubsystem>();
        }
    }
    UE_LOG(LogGameplayTagValue, Error, TEXT("UGameplayTagValueSubsystem::Get() failed: Could not find GameInstance from WorldContextObject."));
    return nullptr;
}

void UGameplayTagValueSubsystem::RegisterRepository(TScriptInterface<ITagValueRepository> Repository)
{
    if (Repository.GetInterface() && Repository.GetObjectRef()) // Ensure it's a valid UObject implementing the interface
    {
        if (!Repositories.Contains(Repository))
        {
            Repositories.Add(Repository);
            SortRepositories();
            UE_LOG(LogGameplayTagValue, Log, TEXT("Repository '%s' (Priority: %d) registered."), *Repository->GetRepositoryName().ToString(), Repository->GetRepositoryPriority());
        }
        else
        {
            UE_LOG(LogGameplayTagValue, Warning, TEXT("Attempted to register repository %s which is already registered."), *Repository->GetRepositoryName().ToString());
        }
    }
    else
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("Attempted to register a null repository interface."));
    }
}

void UGameplayTagValueSubsystem::UnregisterRepository(TScriptInterface<ITagValueRepository> Repository)
{
    if (Repository.GetInterface() && Repository.GetObjectRef())
    {
        const int32 RemovedCount = Repositories.Remove(Repository);
        if (RemovedCount > 0)
        {
            SortRepositories(); // Re-sort if an element was actually removed
            UE_LOG(LogGameplayTagValue, Log, TEXT("Repository '%s' unregistered."), *Repository->GetRepositoryName().ToString());
        }
        else
        {
            UE_LOG(LogGameplayTagValue, Warning, TEXT("Repository '%s' not found for unregistration."), *Repository->GetRepositoryName().ToString());
        }
    }
     else
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("Attempted to unregister a null repository interface."));
    }
}

bool UGameplayTagValueSubsystem::GetInstancedStructValue(FGameplayTag Tag, FInstancedStruct& OutValue) const
{
    if (!Tag.IsValid()) return false;

    // Check UObject repositories first (sorted by priority)
    for (const TScriptInterface<ITagValueRepository>& Repo : Repositories)
    {
        if (Repo && Repo->HasValue(Tag))
        {
            return Repo->GetValue(Tag, OutValue);
        }
    }

    // Check default internal repository if not found in others
    if (DefaultRepositoryInternal.IsValid() && DefaultRepositoryInternal->HasValue(Tag))
    {
        return DefaultRepositoryInternal->GetValue(Tag, OutValue);
    }
    
    UE_LOG(LogGameplayTagValue, Warning, TEXT("GetTagValue_Instanced: Tag %s not found in any repository."), *Tag.ToString());
    return false;
}

void UGameplayTagValueSubsystem::SetInstancedStructValue(FGameplayTag Tag, const FInstancedStruct& InValue, FName TargetRepositoryName)
{
    if (!Tag.IsValid())
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("SetTagValue_Instanced: Tag %s. Value struct is invalid."), *Tag.ToString());
        return;
    }
    
    ITagValueRepository* WritableRepo = GetWritableRepository(TargetRepositoryName);
    if (WritableRepo)
    {
        WritableRepo->SetValue(Tag, InValue);
        UE_LOG(LogGameplayTagValue, Verbose, TEXT("Registering repository: %s with priority %d"), *WritableRepo->GetRepositoryName().ToString(), WritableRepo->GetRepositoryPriority());
    }
    else
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("SetTagValue_Instanced: No repositories available to set value for tag %s."), *Tag.ToString());
    }
}

void UGameplayTagValueSubsystem::GetTagValue_Instanced(FGameplayTag Tag, FInstancedStruct& Value, bool& bSuccess) const
{
    bSuccess = GetInstancedStructValue(Tag, Value);
}

void UGameplayTagValueSubsystem::SetTagValue_Instanced(FGameplayTag Tag, const FInstancedStruct& Value, FName TargetRepositoryName)
{
    SetInstancedStructValue(Tag, Value, TargetRepositoryName);
}

bool UGameplayTagValueSubsystem::HasTagValue(FGameplayTag Tag) const
{
    if (!Tag.IsValid()) return false;

    for (const TScriptInterface<ITagValueRepository>& Repo : Repositories)
    {
        if (Repo && Repo->HasValue(Tag))
        {
            return true;
        }
    }
    if (DefaultRepositoryInternal.IsValid() && DefaultRepositoryInternal->HasValue(Tag))
    {
        return true;
    }
    return false;
}

void UGameplayTagValueSubsystem::ClearTagValue(FGameplayTag Tag, FName TargetRepositoryName)
{
    if (!Tag.IsValid()) return;

    ITagValueRepository* WritableRepo = GetWritableRepository(TargetRepositoryName);
    if (WritableRepo)
    {
        WritableRepo->ClearValue(Tag);
        UE_LOG(LogGameplayTagValue, Verbose, TEXT("Tag '%s' cleared from repository '%s'."), *Tag.ToString(), *WritableRepo->GetRepositoryName().ToString());
    }
    else
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("SetTagValue_Instanced: Could not find repository named %s to set value for tag %s."), *TargetRepositoryName.ToString(), *Tag.ToString());
    }
}

void UGameplayTagValueSubsystem::ClearAllTagValues(FName TargetRepositoryName)
{
    ITagValueRepository* WritableRepo = GetWritableRepository(TargetRepositoryName);
    if (WritableRepo)
    {
        WritableRepo->ClearAllValues();
        UE_LOG(LogGameplayTagValue, Verbose, TEXT("All values cleared from repository '%s'."), *WritableRepo->GetRepositoryName().ToString());
    }
}

void UGameplayTagValueSubsystem::SortRepositories()
{
    Repositories.Sort([](const TScriptInterface<ITagValueRepository>& A, const TScriptInterface<ITagValueRepository>& B) {
        if (!A.GetInterface() || !B.GetInterface()) // Should not happen if registration is done correctly
        {
            return !A.GetInterface(); // Nulls go to the end
        }
        return A->GetRepositoryPriority() > B->GetRepositoryPriority(); // Sort descending by priority
    });
}

ITagValueRepository* UGameplayTagValueSubsystem::GetWritableRepository(FName RepositoryName) const
{
    if (RepositoryName != NAME_None)
    {
        // Find specific UObject repository
        for (const TScriptInterface<ITagValueRepository>& Repo : Repositories)
        {
            if (Repo && Repo->GetRepositoryName() == RepositoryName)
            {
                return Repo.GetInterface();
            }
        }
        // Check if it's the default internal repository
        if (DefaultRepositoryInternal.IsValid() && DefaultRepositoryInternal->GetRepositoryName() == RepositoryName)
        {
            return DefaultRepositoryInternal.Get();
        }
    }
    else // If NAME_None, find the highest priority writable one
    {
        // Check UObject repositories first
        if (Repositories.Num() > 0 && Repositories[0].GetInterface())
        {
             // Assuming sorted, the first one is highest priority among UObject repos
            return Repositories[0].GetInterface();
        }
        // Fallback to default internal repository if no UObject repositories or highest priority one is not valid
        if (DefaultRepositoryInternal.IsValid())
        {
            // This logic needs refinement: if there are UObject repos, we should compare their priority
            // with DefaultRepositoryInternal's priority. For now, it prioritizes any registered UObject repo.
            // A better approach would be to include the DefaultRepositoryInternal in the sorting logic
            // perhaps via a wrapper or by having a combined list for GetValue operations.
            // For SetValue with NAME_None, we need a clear rule. Let's assume for now it targets the
            // highest priority registered UObject repo, or the default internal if none are registered.
            if (Repositories.Num() == 0)
            {
                 return DefaultRepositoryInternal.Get();
            }
        }
    }
    return nullptr;
}
