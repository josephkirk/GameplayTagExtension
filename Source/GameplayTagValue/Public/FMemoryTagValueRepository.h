// Copyright Nguyen Phi Hung 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ITagValueRepository.h" // The interface it implements
#include "GameplayTagContainer.h"
#include "UObject/StructOnScope.h" // For FInstancedStruct


/**
 * A simple in-memory implementation of the ITagValueRepository interface.
 * This is not a UCLASS/USTRUCT and is intended for direct C++ usage,
 * typically managed by a UObject (like a subsystem or component).
 */
class GAMEPLAYTAGVALUE_API FMemoryTagValueRepository : public ITagValueRepository
{
public:
    FMemoryTagValueRepository(FName InRepositoryName, int32 InPriority);
    virtual ~FMemoryTagValueRepository() = default;

    //~ Begin ITagValueRepository Interface
    virtual bool GetValue(FGameplayTag Tag, FInstancedStruct& OutValue) const override;
    virtual void SetValue(FGameplayTag Tag, const FInstancedStruct& InValue) override;
    virtual bool HasValue(FGameplayTag Tag) const override;
    virtual void ClearValue(FGameplayTag Tag) override;
    virtual void ClearAllValues() override;
    virtual FName GetRepositoryName() const override;
    virtual int32 GetRepositoryPriority() const override;
    //~ End ITagValueRepository Interface

private:
    TMap<FGameplayTag, FInstancedStruct> TagValues;
    FName RepositoryName;
    int32 Priority;
};
