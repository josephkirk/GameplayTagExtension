// Copyright Nguyen Phi Hung 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ITagValueRepository.h" // The interface it implements
#include "GameplayTagContainer.h"
#include "UObject/StructOnScope.h" // For FInstancedStruct
#include "TagValueRepositoryDataAsset.generated.h"

/**
 * Data asset that implements the ITagValueRepository interface.
 * Can be used to define a static set of gameplay tag values that can be loaded and accessed at runtime.
 */
UCLASS(BlueprintType)
class GAMEPLAYTAGVALUE_API UTagValueRepositoryDataAsset : public UDataAsset, public ITagValueRepository
{
    GENERATED_BODY()

public:
    UTagValueRepositoryDataAsset();

    //~ Begin ITagValueRepository Interface
    virtual bool GetValue(FGameplayTag Tag, FInstancedStruct& OutValue) const override;
    virtual void SetValue(FGameplayTag Tag, const FInstancedStruct& InValue) override;
    virtual bool HasValue(FGameplayTag Tag) const override;
    virtual void ClearValue(FGameplayTag Tag) override;
    virtual void ClearAllValues() override;
    virtual FName GetRepositoryName() const override;
    virtual int32 GetRepositoryPriority() const override;
    //~ End ITagValueRepository Interface

    /** Sets a value directly in this data asset's storage. The input struct type is determined by what you connect to 'Value'. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue|DataAsset", meta = (DisplayName = "Set Data Asset Tag Value (Instanced)", AutoCreateRefTerm = "Tag,Value"))
    void SetDataAssetTagValue(FGameplayTag Tag, const FInstancedStruct& Value);

    /** Gets a value directly from this data asset's storage. The output struct type is determined by what you connect to 'Value'. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue|DataAsset", meta = (DisplayName = "Get Data Asset Tag Value (Instanced)", ExpandBoolAsExecs = "bSuccess", AutoCreateRefTerm = "Tag"))
    void GetDataAssetTagValue(FGameplayTag Tag, UPARAM(ref) FInstancedStruct& Value, bool& bSuccess) const;

    /** Checks if a value exists directly in this data asset's storage. */
    UFUNCTION(BlueprintPure, Category = "GameplayTagValue|DataAsset", meta=(AutoCreateRefTerm = "Tag"))
    bool HasDataAssetTagValue(FGameplayTag Tag) const;

    /** Registers this data asset with the GameplayTagValueSubsystem. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue|DataAsset")
    void RegisterWithSubsystem(UObject* WorldContextObject);

    /** Unregisters this data asset from the GameplayTagValueSubsystem. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue|DataAsset")
    void UnregisterFromSubsystem(UObject* WorldContextObject);

public:
    /** Name of this repository, used for identification by the subsystem. If None, a unique name will be generated. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTagValue")
    FName RepositoryName;

    /** Priority of this repository. Higher values are checked first by the subsystem. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTagValue")
    int32 Priority;

    /** If true, this data asset will automatically register itself with the GameplayTagValueSubsystem when loaded. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTagValue")
    bool bAutoRegisterWithSubsystem;

protected:
    /** The actual storage for tag values. Exposed to editor for configuration. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTagValue", meta = (DisplayName = "Tag Values Storage"))
    TMap<FGameplayTag, FInstancedStruct> DataAssetTagValues;

private:
    bool bIsRegisteredWithSubsystem;
};
