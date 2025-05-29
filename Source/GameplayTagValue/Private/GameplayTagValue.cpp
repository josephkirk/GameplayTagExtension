// Copyright Nguyen Phi Hung 2025. All Rights Reserved.

#include "GameplayTagValue.h" // Public header for the module

// Define the log category
DEFINE_LOG_CATEGORY(LogGameplayTagValue);

#define LOCTEXT_NAMESPACE "FGameplayTagValueModule" // For localization, if needed

void FGameplayTagValueModule::StartupModule()
{
    // This code will execute after your module is loaded into memory;
    // the exact timing is specified in the .uplugin file per-module (if it's a plugin)
    // or by other means if it's a game module.
    UE_LOG(LogGameplayTagValue, Log, TEXT("GameplayTagValue module has started."));
}

void FGameplayTagValueModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.
    // For modules that support dynamic reloading, we call this function before unloading the module.
    UE_LOG(LogGameplayTagValue, Log, TEXT("GameplayTagValue module has been shut down."));
}

#undef LOCTEXT_NAMESPACE

// Implements the module. This is necessary for the engine to find and load the module.
IMPLEMENT_MODULE(FGameplayTagValueModule, GameplayTagValue)
