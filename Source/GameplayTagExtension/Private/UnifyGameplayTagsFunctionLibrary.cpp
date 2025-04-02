#include "UnifyGameplayTagsFunctionLibrary.h"
#include "GameFramework/GameplayMessageSubsystem.h"

void UUnifyGameplayTagsFunctionLibrary::BroadcastGameplayTagMessage(const UObject* WorldContextObject,const FGameplayTag Channel, const FInstancedStruct& MessagePayload)
{
    UGameplayMessageSubsystem::Get(WorldContextObject).BroadcastMessage(Channel, MessagePayload);
}
