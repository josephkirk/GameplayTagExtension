// Copyright Windsurf Engineering, 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "StructUtils/InstancedStruct.h"
#include "ITagValueRepository.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UTagValueRepository : public UInterface
{
    GENERATED_BODY()
};

class GAMEPLAYTAGVALUE_API ITagValueRepository
{
    GENERATED_BODY()

public:
    /**
     * Gets the value associated with the given tag.
     * @param Tag The tag to query.
     * @param OutValue The FInstancedStruct to populate if the tag is found.
     * @return True if the tag was found and OutValue is populated, false otherwise.
     */
    virtual bool GetValue(FGameplayTag Tag, FInstancedStruct& OutValue) const = 0;

    /**
     * Sets the value for the given tag.
     * @param Tag The tag to associate the value with.
     * @param InValue The FInstancedStruct containing the value to set.
     */
    virtual void SetValue(FGameplayTag Tag, const FInstancedStruct& InValue) = 0;

    /**
     * Checks if a value is associated with the given tag.
     * @param Tag The tag to query.
     * @return True if a value exists for the tag, false otherwise.
     */
    virtual bool HasValue(FGameplayTag Tag) const = 0;

    /**
     * Removes the value associated with the given tag.
     * @param Tag The tag whose value should be cleared.
     */
    virtual void ClearValue(FGameplayTag Tag) = 0;

    /**
     * Removes all values from this repository.
     */
    virtual void ClearAllValues() = 0;

    /**
     * Gets the unique name of this repository.
     * @return The name of the repository.
     */
    virtual FName GetRepositoryName() const = 0;

    /**
     * Gets the priority of this repository. Higher values are checked first.
     * @return The priority of the repository.
     */
    virtual int32 GetRepositoryPriority() const = 0;

    // Templated helper to get a specific struct type
    template<typename T>
    bool GetValue(FGameplayTag Tag, T& OutValue) const
    {
        FInstancedStruct InstancedStruct;
        if (GetValue(Tag, InstancedStruct))
        {
            if (InstancedStruct.IsValid() && InstancedStruct.GetScriptStruct()->IsChildOf(T::StaticStruct()))
            {
                OutValue = *InstancedStruct.GetPtr<T>();
                return true;
            }
        }
        return false;
    }

    // Templated helper to set a specific struct type
    template<typename T>
    void SetValue(FGameplayTag Tag, const T& InValue)
    {
        FInstancedStruct InstancedStruct;
        InstancedStruct.InitializeAs(T::StaticStruct(), reinterpret_cast<const uint8*>(&InValue));
        SetValue(Tag, InstancedStruct);
    }
};
