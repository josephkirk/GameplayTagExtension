// Copyright Nguyen Phi Hung 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "UObject/StructOnScope.h" // For FInstancedStruct
#include "ITagValueRepository.h"   // For the interface
#include "GameplayTagValueSubsystem.generated.h"

// Forward declarations
class FMemoryTagValueRepository;

UCLASS(Config=Game) // Expose properties to config files (e.g., DefaultGame.ini)
class GAMEPLAYTAGVALUE_API UGameplayTagValueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    //~ Begin USubsystem Interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    //~ End USubsystem Interface

    /** Gets the subsystem instance, primarily for C++ usage. */
    static UGameplayTagValueSubsystem* Get(const UObject* WorldContextObject);

    // --- Repository Management ---
    /** Registers a repository with the subsystem. The subsystem does not take ownership of UObject repositories. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue|Repository")
    void RegisterRepository(TScriptInterface<ITagValueRepository> Repository);

    /** Unregisters a repository from the subsystem. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue|Repository")
    void UnregisterRepository(TScriptInterface<ITagValueRepository> Repository);

    // --- Value Access (FInstancedStruct - for C++ and advanced BP) ---

    /**
     * Gets the value associated with the given tag, searching through repositories by priority.
     * @param Tag The tag to query.
     * @param OutValue The FInstancedStruct to populate if the tag is found.
     * @return True if the tag was found and OutValue is populated, false otherwise.
     */
    bool GetInstancedStructValue(FGameplayTag Tag, FInstancedStruct& OutValue) const;

    /**
     * Sets the value for the given tag.
     * If TargetRepositoryName is NAME_None, it sets the value in the highest priority writable repository (or default).
     * @param Tag The tag to associate the value with.
     * @param InValue The FInstancedStruct containing the value to set.
     * @param TargetRepositoryName Optional: The name of the repository to set the value in.
     */
    void SetInstancedStructValue(FGameplayTag Tag, const FInstancedStruct& InValue, FName TargetRepositoryName = NAME_None);

    // --- Value Access (Templated - for C++) ---

    template<typename T>
    bool GetValue(FGameplayTag Tag, T& OutValue) const
    {
        FInstancedStruct InstancedStruct;
        if (GetInstancedStructValue(Tag, InstancedStruct))
        {
            if (InstancedStruct.IsValid() && InstancedStruct.GetScriptStruct() && InstancedStruct.GetScriptStruct()->IsChildOf(T::StaticStruct()))
            {
                OutValue = *InstancedStruct.GetPtr<T>();
                return true;
            }
        }
        return false;
    }

    template<typename T>
    void SetValue(FGameplayTag Tag, const T& InValue, FName TargetRepositoryName = NAME_None)
    {
        FInstancedStruct InstancedStruct;
        InstancedStruct.InitializeAs(T::StaticStruct(), reinterpret_cast<const uint8*>(&InValue));
        SetInstancedStructValue(Tag, InstancedStruct, TargetRepositoryName);
    }

    // --- Value Access (Blueprint - using wildcard pins) ---

    /**
     * Gets the value associated with the given tag. The output struct type is determined by what you connect to 'Value'.
     * @param Tag The tag to query.
     * @param Value The FInstancedStruct to populate. Note: This is an FInstancedStruct, not the raw struct type.
     * @param bSuccess True if the tag was found and value retrieved successfully.
     */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue", meta = (DisplayName = "Get Tag Value (Instanced)", ExpandBoolAsExecs = "bSuccess", AutoCreateRefTerm = "Tag"))
    void GetTagValue_Instanced(FGameplayTag Tag, UPARAM(ref) FInstancedStruct& Value, bool& bSuccess) const;

    /**
     * Sets the value for the given tag. The input struct type is determined by what you connect to 'Value'.
     * @param Tag The tag to associate the value with.
     * @param Value The FInstancedStruct containing the value to set.
     * @param TargetRepositoryName Optional: The name of the repository to set the value in. If NAME_None, uses the default/highest priority.
     */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue", meta = (DisplayName = "Set Tag Value (Instanced)", AutoCreateRefTerm = "Tag,Value"))
    void SetTagValue_Instanced(FGameplayTag Tag, const FInstancedStruct& Value, FName TargetRepositoryName = NAME_None);


    /** Checks if a value exists for the given tag in any repository. */
    UFUNCTION(BlueprintPure, Category = "GameplayTagValue", meta=(AutoCreateRefTerm = "Tag"))
    bool HasTagValue(FGameplayTag Tag) const;

    /** Clears a value for the given tag from a specific repository. If TargetRepositoryName is NAME_None, it attempts the default/highest priority. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue", meta=(AutoCreateRefTerm = "Tag"))
    void ClearTagValue(FGameplayTag Tag, FName TargetRepositoryName = NAME_None);

    /** Clears all values from a specific repository. If TargetRepositoryName is NAME_None, it attempts the default/highest priority. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue")
    void ClearAllTagValues(FName TargetRepositoryName = NAME_None);


protected:
    /** Sorts repositories by priority (descending). To be called after registration/unregistration or priority changes. */
    void SortRepositories();

    /** Finds a writable repository. If Name is None, returns the highest priority one. */
    ITagValueRepository* GetWritableRepository(FName RepositoryName) const;

private:
    UPROPERTY()
    TArray<TScriptInterface<ITagValueRepository>> Repositories;

    /** Default in-memory repository managed by the subsystem. This is always present. */
    TSharedPtr<FMemoryTagValueRepository> DefaultRepositoryInternal;

    // Configuration for the default internal repository
    UPROPERTY(Config, EditAnywhere, Category = "Default Repository")
    FName DefaultInternalRepositoryName = FName(TEXT("DefaultSubsystemRepository"));

    UPROPERTY(Config, EditAnywhere, Category = "Default Repository")
    int32 DefaultInternalRepositoryPriority = 0; // Typically a lower priority
};
