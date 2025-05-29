// Copyright Windsurf Engineering, 2025. All Rights Reserved.

#include "TagValueRepositoryComponent.h"
#include "GameplayTagValue.h" // For LogGameplayTagValue
#include "GameplayTagValueSubsystem.h" // To register/unregister
#include "Engine/World.h"
#include "GameFramework/Actor.h" // For GetOwner()

UTagValueRepositoryComponent::UTagValueRepositoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false; // No ticking needed by default
    RepositoryName = NAME_None; // Default to None, will generate a unique one if needed
    Priority = 100; // Default priority, common for actor-specific values
    bRegisterWithSubsystemOnBeginPlay = true;
    bIsRegisteredWithSubsystem = false;
}

void UTagValueRepositoryComponent::BeginPlay()
{
    Super::BeginPlay();
    if (bRegisterWithSubsystemOnBeginPlay)
    {
        TryRegisterWithSubsystem();
    }
}

void UTagValueRepositoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    TryUnregisterFromSubsystem(); // Always try to unregister
    Super::EndPlay(EndPlayReason);
}

void UTagValueRepositoryComponent::TryRegisterWithSubsystem()
{
    if (bIsRegisteredWithSubsystem) return;

    if (UGameplayTagValueSubsystem* Subsystem = UGameplayTagValueSubsystem::Get(GetWorld()))
    {
        TScriptInterface<ITagValueRepository> ThisAsRepo;
        ThisAsRepo.SetObject(this);
        // The interface pointer is automatically found if the UObject implements it.
        // ThisAsRepo.SetInterface(Cast<ITagValueRepository>(this)); // Not strictly necessary for UObject based interfaces

        if (ThisAsRepo.GetInterface() != nullptr) // Check if the interface is valid
        {
            Subsystem->RegisterRepository(ThisAsRepo);
            bIsRegisteredWithSubsystem = true;
        }
        else { UE_LOG(LogGameplayTagValue, Warning, TEXT("UTagValueRepositoryComponent %s on Actor %s failed to cast to ITagValueRepository for registration."), *GetName(), GetOwner() ? *GetOwner()->GetName() : TEXT("None")); }
    }
    else { UE_LOG(LogGameplayTagValue, Warning, TEXT("UTagValueRepositoryComponent %s on Actor %s could not find GameplayTagValueSubsystem for registration."), *GetName(), GetOwner() ? *GetOwner()->GetName() : TEXT("None")); }
}

void UTagValueRepositoryComponent::TryUnregisterFromSubsystem()
{
    if (!bIsRegisteredWithSubsystem) return;

    if (UGameplayTagValueSubsystem* Subsystem = UGameplayTagValueSubsystem::Get(GetWorld()))
    {
        TScriptInterface<ITagValueRepository> ThisAsRepo;
        ThisAsRepo.SetObject(this);
        // ThisAsRepo.SetInterface(Cast<ITagValueRepository>(this));

        if (ThisAsRepo.GetInterface() != nullptr)
        {
            Subsystem->UnregisterRepository(ThisAsRepo);
            bIsRegisteredWithSubsystem = false;
        }
    }
}

// ITagValueRepository Implementation
bool UTagValueRepositoryComponent::GetValue(FGameplayTag Tag, FInstancedStruct& OutValue) const
{
    const FInstancedStruct* FoundValue = ComponentTagValues.Find(Tag);
    if (FoundValue && FoundValue->IsValid())
    {
        OutValue = *FoundValue; // FInstancedStruct supports copy
        return true;
    }
    return false;
}

void UTagValueRepositoryComponent::SetValue(FGameplayTag Tag, const FInstancedStruct& InValue)
{
    if (!Tag.IsValid())
    {
        UE_LOG(LogGameplayTagValue, Warning, TEXT("UTagValueRepositoryComponent %s: Attempted to set value for invalid tag."), *GetRepositoryName().ToString());
        return;
    }
    UE_LOG(LogGameplayTagValue, Verbose, TEXT("UTagValueRepositoryComponent %s: Setting tag %s."), *GetRepositoryName().ToString(), *Tag.ToString());
    ComponentTagValues.Emplace(Tag, InValue);
}

bool UTagValueRepositoryComponent::HasValue(FGameplayTag Tag) const
{
    const FInstancedStruct* FoundValue = ComponentTagValues.Find(Tag);
    return FoundValue && FoundValue->IsValid();
}

void UTagValueRepositoryComponent::ClearValue(FGameplayTag Tag)
{
    ComponentTagValues.Remove(Tag);
}

void UTagValueRepositoryComponent::ClearAllValues()
{
    ComponentTagValues.Empty();
}

FName UTagValueRepositoryComponent::GetRepositoryName() const
{
    if (RepositoryName.IsNone())
    {
        // Generate a somewhat unique name if not set
        if (AActor* Owner = GetOwner())
        {
            return FName(*FString::Printf(TEXT("%s_%s_Repo"), *Owner->GetName(), *GetName()));
        }
        return FName(*FString::Printf(TEXT("%s_Repo"), *GetName()));
    }
    return RepositoryName;
}

int32 UTagValueRepositoryComponent::GetRepositoryPriority() const
{
    return Priority;
}

// Component-specific Blueprint functions
void UTagValueRepositoryComponent::SetComponentTagValue(FGameplayTag Tag, const FInstancedStruct& Value)
{
    SetValue(Tag, Value); // Directly calls the interface method implemented by this component
}

void UTagValueRepositoryComponent::GetComponentTagValue(FGameplayTag Tag, FInstancedStruct& Value, bool& bSuccess) const
{
    bSuccess = GetValue(Tag, Value); // Directly calls the interface method
}

bool UTagValueRepositoryComponent::HasComponentTagValue(FGameplayTag Tag) const
{
    return HasValue(Tag); // Directly calls the interface method
}
