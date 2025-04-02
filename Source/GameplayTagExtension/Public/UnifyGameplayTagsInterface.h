// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Interface.h"
#include "UnifyGameplayTagsInterface.generated.h"

/**
 * Message struct for UnifyGameplayTags communication
 */
USTRUCT(BlueprintType)
struct GAMEPLAYTAGEXTENSION_API FUnifyGameplayTag
{
	GENERATED_BODY()

	/** The object that broadcast the message */
	UPROPERTY(BlueprintReadWrite, Category = "GameplayTags")
	UObject* SourceObject = nullptr;

	/** Optional payload for the event. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FInstancedStruct Payload;

	/** Container of gameplay tags being communicated */
	UPROPERTY(BlueprintReadWrite, Category = "GameplayTags")
	FGameplayTagContainer Tags;
};

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class GAMEPLAYTAGEXTENSION_API UUnifyGameplayTagsInterface : public UInterface
{
	GENERATED_BODY()
};

// Add interface functions to this class. This is the class that will be inherited to implement this interface.
class GAMEPLAYTAGEXTENSION_API IUnifyGameplayTagsInterface
{
	GENERATED_BODY()

public:
	/** Gets all gameplay tags in the implementing object's tag container */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	FGameplayTagContainer GetGameplayTagContainer() const;

	/** Sets the implementing object's tag container */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	void SetGameplayTagContainer(const FGameplayTagContainer& NewTagContainer);

	/** Adds the specified gameplay tag to the implementing object's tag container */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	void AddGameplayTag(const FGameplayTag& TagToAdd);

	/** Adds the specified gameplay tags to the implementing object's tag container */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	void AddGameplayTags(const FGameplayTagContainer& TagsToAdd);

	/** Removes the specified gameplay tag from the implementing object's tag container */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	void RemoveGameplayTag(const FGameplayTag& TagToRemove);

	/** Removes the specified gameplay tags from the implementing object's tag container */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	void RemoveGameplayTags(const FGameplayTagContainer& TagsToRemove);

	/** Checks if the implementing object's tag container has the specified tag */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	bool HasGameplayTag(const FGameplayTag& TagToCheck) const;

	/** Checks if the implementing object's tag container has all of the specified tags */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	bool HasAllGameplayTags(const FGameplayTagContainer& TagsToCheck) const;

	/** Checks if the implementing object's tag container has any of the specified tags */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	bool HasAnyGameplayTags(const FGameplayTagContainer& TagsToCheck) const;

	/** Clears all gameplay tags in the implementing object's tag container */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameplayTags")
	void ClearGameplayTags();

};
