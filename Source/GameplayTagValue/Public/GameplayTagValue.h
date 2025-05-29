// Copyright Nguyen Phi Hung 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/** Log category for the GameplayTagValue module. */
DECLARE_LOG_CATEGORY_EXTERN(LogGameplayTagValue, Log, All);

/**
 * The public interface to this module.
 */
class FGameplayTagValueModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
