#pragma once

#include "GameplayTagContainer.h"
#include "GameplayTags.h"
#include "UnifyGameplayTagsInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnifyGameplayTagsFunctionLibrary.generated.h"

/**
 * Function library for UnifyGameplayTags
 */
UCLASS()
class GAMEPLAYTAGEXTENSION_API UUnifyGameplayTagsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "GameplayTags")
    static void BroadcastGameplayTagMessage(const UObject* WorldContextObject,const FGameplayTag Channel, const FInstancedStruct& MessagePayload);
};