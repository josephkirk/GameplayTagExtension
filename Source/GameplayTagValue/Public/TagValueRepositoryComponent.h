// Copyright Nguyen Phi Hung 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ITagValueRepository.h" // The interface it implements
#include "GameplayTagContainer.h"
#include "UObject/StructOnScope.h" // For FInstancedStruct
#include "TagValueRepositoryComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYTAGVALUE_API UTagValueRepositoryComponent : public UActorComponent, public ITagValueRepository
{
    GENERATED_BODY()

public:
    UTagValueRepositoryComponent();

protected:
    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End UActorComponent Interface

public:
    //~ Begin ITagValueRepository Interface
    virtual bool GetValue(FGameplayTag Tag, FInstancedStruct& OutValue) const override;
    virtual void SetValue(FGameplayTag Tag, const FInstancedStruct& InValue) override;
    virtual bool HasValue(FGameplayTag Tag) const override;
    virtual void ClearValue(FGameplayTag Tag) override;
    virtual void ClearAllValues() override;
    virtual FName GetRepositoryName() const override;
    virtual int32 GetRepositoryPriority() const override;
    //~ End ITagValueRepository Interface

    /** Sets a value directly in this component's storage. The input struct type is determined by what you connect to 'Value'. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue|Component", meta = (DisplayName = "Set Component Tag Value (Instanced)", AutoCreateRefTerm = "Tag,Value"))
    void SetComponentTagValue(FGameplayTag Tag, const FInstancedStruct& Value);

    /** Gets a value directly from this component's storage. The output struct type is determined by what you connect to 'Value'. */
    UFUNCTION(BlueprintCallable, Category = "GameplayTagValue|Component", meta = (DisplayName = "Get Component Tag Value (Instanced)", ExpandBoolAsExecs = "bSuccess", AutoCreateRefTerm = "Tag"))
    void GetComponentTagValue(FGameplayTag Tag, UPARAM(ref) FInstancedStruct& Value, bool& bSuccess) const;

    /** Checks if a value exists directly in this component's storage. */
    UFUNCTION(BlueprintPure, Category = "GameplayTagValue|Component", meta=(AutoCreateRefTerm = "Tag"))
    bool HasComponentTagValue(FGameplayTag Tag) const;

public:
    /** Name of this repository, used for identification by the subsystem. If None, a unique name will be generated. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTagValue")
    FName RepositoryName;

    /** Priority of this repository. Higher values are checked first by the subsystem. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTagValue")
    int32 Priority;

    /** If true, this component will automatically register itself with the GameplayTagValueSubsystem on BeginPlay. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTagValue")
    bool bRegisterWithSubsystemOnBeginPlay;

protected:
    /** The actual storage for tag values. Exposed to editor for pre-configuration. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTagValue", meta = (DisplayName = "Tag Values Storage"))
    TMap<FGameplayTag, FInstancedStruct> ComponentTagValues;

private:
    void TryRegisterWithSubsystem();
    void TryUnregisterFromSubsystem();
    
    bool bIsRegisteredWithSubsystem;
};
