// Copyright Nguyen Phi Hung 2025. All Rights Reserved.

#include "TagValueRepositoryDataAsset.h"
#include "GameplayTagValue.h" // For LogGameplayTagValue
#include "GameplayTagValueSubsystem.h" // To register/unregister
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UTagValueRepositoryDataAsset::UTagValueRepositoryDataAsset()
{
    RepositoryName = NAME_None; // Default to None, will generate a unique one if needed
    Priority = 200; // Default priority higher than components (100) since data assets are typically more global
    bAutoRegisterWithSubsystem = true;
    bIsRegisteredWithSubsystem = false;
}

// ITagValueRepository Implementation
bool UTagValueRepositoryDataAsset::GetValue(FGameplayTag Tag, FInstancedStruct& OutValue) const
{
    const FInstancedStruct* FoundValue = DataAssetTagValues.Find(Tag);
    if (FoundValue && FoundValue->IsValid())
    {
        OutValue = *FoundValue; // FInstancedStruct supports copy
        return true;
    }
    return false;
}

void UTagValueRepositoryDataAsset::SetValue(FGameplayTag Tag, const FInstancedStruct& InValue)
{
    if (!Tag.IsValid())
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("UTagValueRepositoryDataAsset %s: Attempted to set value for invalid tag."), *GetRepositoryName().ToString());
        return;
    }
    UE_LOG(LogGameplayTagValue, Verbose, TEXT("UTagValueRepositoryDataAsset %s: Setting tag %s."), *GetRepositoryName().ToString(), *Tag.ToString());
    DataAssetTagValues.Emplace(Tag, InValue);
}

bool UTagValueRepositoryDataAsset::HasValue(FGameplayTag Tag) const
{
    const FInstancedStruct* FoundValue = DataAssetTagValues.Find(Tag);
    return FoundValue && FoundValue->IsValid();
}

void UTagValueRepositoryDataAsset::ClearValue(FGameplayTag Tag)
{
    DataAssetTagValues.Remove(Tag);
}

void UTagValueRepositoryDataAsset::ClearAllValues()
{
    DataAssetTagValues.Empty();
}

FName UTagValueRepositoryDataAsset::GetRepositoryName() const
{
    if (RepositoryName.IsNone())
    {
        // Generate a somewhat unique name if not set
        return FName(*FString::Printf(TEXT("%s_DataAsset_Repo"), *GetName()));
    }
    return RepositoryName;
}

int32 UTagValueRepositoryDataAsset::GetRepositoryPriority() const
{
    return Priority;
}

// DataAsset-specific Blueprint functions
void UTagValueRepositoryDataAsset::SetDataAssetTagValue(FGameplayTag Tag, const FInstancedStruct& Value)
{
    SetValue(Tag, Value); // Directly calls the interface method implemented by this data asset
}

void UTagValueRepositoryDataAsset::GetDataAssetTagValue(FGameplayTag Tag, FInstancedStruct& Value, bool& bSuccess) const
{
    bSuccess = GetValue(Tag, Value); // Directly calls the interface method
}

bool UTagValueRepositoryDataAsset::HasDataAssetTagValue(FGameplayTag Tag) const
{
    return HasValue(Tag); // Directly calls the interface method
}

void UTagValueRepositoryDataAsset::RegisterWithSubsystem(UObject* WorldContextObject)
{
    if (bIsRegisteredWithSubsystem) return;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("UTagValueRepositoryDataAsset %s: Failed to register - invalid world context."), *GetName());
        return;
    }

    if (UGameplayTagValueSubsystem* Subsystem = UGameplayTagValueSubsystem::Get(World))
    {
        TScriptInterface<ITagValueRepository> ThisAsRepo;
        ThisAsRepo.SetObject(this);
        // The interface pointer is automatically found if the UObject implements it.

        if (ThisAsRepo.GetInterface() != nullptr) // Check if the interface is valid
        {
            Subsystem->RegisterRepository(ThisAsRepo);
            bIsRegisteredWithSubsystem = true;
            UE_LOG(LogGameplayTagValue, Log, TEXT("UTagValueRepositoryDataAsset %s: Successfully registered with GameplayTagValueSubsystem."), *GetName());
        }
        else { UE_LOG(LogGameplayTagValue, Warning, TEXT("UTagValueRepositoryDataAsset %s failed to cast to ITagValueRepository for registration."), *GetName()); }
    }
    else { UE_LOG(LogGameplayTagValue, Warning, TEXT("UTagValueRepositoryDataAsset %s could not find GameplayTagValueSubsystem for registration."), *GetName()); }
}

void UTagValueRepositoryDataAsset::UnregisterFromSubsystem(UObject* WorldContextObject)
{
    if (!bIsRegisteredWithSubsystem) return;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("UTagValueRepositoryDataAsset %s: Failed to unregister - invalid world context."), *GetName());
        return;
    }

    if (UGameplayTagValueSubsystem* Subsystem = UGameplayTagValueSubsystem::Get(World))
    {
        TScriptInterface<ITagValueRepository> ThisAsRepo;
        ThisAsRepo.SetObject(this);

        if (ThisAsRepo.GetInterface() != nullptr)
        {
            Subsystem->UnregisterRepository(ThisAsRepo);
            bIsRegisteredWithSubsystem = false;
            UE_LOG(LogGameplayTagValue, Log, TEXT("UTagValueRepositoryDataAsset %s: Successfully unregistered from GameplayTagValueSubsystem."), *GetName());
        }
    }
}
