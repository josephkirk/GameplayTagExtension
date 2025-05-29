// Copyright Nguyen Phi Hung 2025. All Rights Reserved.

#include "FMemoryTagValueRepository.h" // Corresponding header
#include "UObject/StructOnScope.h"     // For FInstancedStruct methods


FMemoryTagValueRepository::FMemoryTagValueRepository(FName InRepositoryName, int32 InPriority)
    : RepositoryName(InRepositoryName)
    , Priority(InPriority)
{
}

bool FMemoryTagValueRepository::GetValue(FGameplayTag Tag, FInstancedStruct& OutValue) const
{
    const FInstancedStruct* FoundValue = TagValues.Find(Tag);
    if (FoundValue && FoundValue->IsValid())
    {
        OutValue = *FoundValue; // FInstancedStruct supports copy
        return true;
    }
    // UE_LOG(LogGameplayTagValue, Verbose, TEXT("Tag '%s' not found in repository '%s'."), *Tag.ToString(), *RepositoryName.ToString());
    return false;
}

void FMemoryTagValueRepository::SetValue(FGameplayTag Tag, const FInstancedStruct& InValue)
{
    if (!Tag.IsValid())
    {
        // UE_LOG(LogGameplayTagValue, Warning, TEXT("Attempted to set value for invalid tag in repository '%s'."), *RepositoryName.ToString());
        return;
    }
    if (!InValue.IsValid())
    {
        // UE_LOG(LogGameplayTagValue, Warning, TEXT("Attempted to set invalid FInstancedStruct for tag '%s' in repository '%s'."), *Tag.ToString(), *RepositoryName.ToString());
        // Consider whether to remove an existing valid value if an invalid one is provided, or just ignore.
        // Current behavior: an invalid InValue will emplace an invalid FInstancedStruct, GetValue/HasValue will report false.
    }
    TagValues.Emplace(Tag, InValue);
    // UE_LOG(LogGameplayTagValue, Verbose, TEXT("Tag '%s' set in repository '%s'."), *Tag.ToString(), *RepositoryName.ToString());
}

bool FMemoryTagValueRepository::HasValue(FGameplayTag Tag) const
{
    const FInstancedStruct* FoundValue = TagValues.Find(Tag);
    return FoundValue && FoundValue->IsValid();
}

void FMemoryTagValueRepository::ClearValue(FGameplayTag Tag)
{
    TagValues.Remove(Tag);
    // UE_LOG(LogGameplayTagValue, Verbose, TEXT("Tag '%s' cleared from repository '%s'."), *Tag.ToString(), *RepositoryName.ToString());
}

void FMemoryTagValueRepository::ClearAllValues()
{
    TagValues.Empty();
    // UE_LOG(LogGameplayTagValue, Verbose, TEXT("All values cleared from repository '%s'."), *RepositoryName.ToString());
}

FName FMemoryTagValueRepository::GetRepositoryName() const
{
    return RepositoryName;
}

int32 FMemoryTagValueRepository::GetRepositoryPriority() const
{
    return Priority;
}
