#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTags.h"
#include "UnifyGameplayTagsComponent.h"
#include "UnifyGameplayTagsInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnifyGameplayTagsFunctionLibrary.generated.h"

UENUM()
enum EGameplayTagCheckType : uint8
{
    Any,
    Exact
};

/**
 * Function library for UnifyGameplayTags
 */
UCLASS()
class GAMEPLAYTAGEXTENSION_API UUnifyGameplayTagsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta=(WorldContext="WorldContextObject"))
	static void BroadcastGameplayTagMessage(const UObject* WorldContextObject,const FGameplayTag Channel, const FInstancedStruct& MessagePayload);

	/** Get the gameplay tag component for the specified actor */
	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	static UUnifyGameplayTagsComponent* GetGameplayTagComponent(const AActor* Actor);

	/** Get the gameplay tag container for the specified actor */
	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	static FGameplayTagContainer GetGameplayTagContainer(const AActor* Actor, bool& bSuccess);

	/** Filter actors with gameplay tags */
	UFUNCTION(BlueprintPure, Category = "Actor")
	static TArray<AActor*> FilterActorsWithGameplayTags(const TArray<AActor*>& ActorsToCheck, const FGameplayTagContainer TagsToCheck);

	/** Get all actors with gameplay tags */
	UFUNCTION(BlueprintCallable, Category="Actor",  meta=(WorldContext="WorldContextObject"))
	static void GetAllActorsWithGameplayTags(const UObject* WorldContextObject, const FGameplayTagContainer TagsToCheck, TArray<AActor*>& OutActors);

    /** Check if actor have Gameplay tags with enum check type of any or exact*/
    UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (WorldContext = "WorldContextObject"))
    static bool IsActorHasGameplayTags(const UObject* WorldContextObject, const AActor* Actor, const FGameplayTagContainer TagsToCheck, EGameplayTagCheckType CheckType);

    /** Run set/add/remove/clear on Gameplaytags on Actor with GameplayTag Components with TagChangeType */
    UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (WorldContext = "WorldContextObject"))
    static void RunGameplayTagOperationOnActor(const UObject* WorldContextObject, const AActor* Actor, const FGameplayTagContainer Tags, ETagChangeType OperationType, FGameplayTagContainer& OutContainer, bool& bSuccess);

    /** create event dispatched on actor on gameplaytag change*/
    
};