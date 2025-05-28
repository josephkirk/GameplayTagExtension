# Gameplay Tag Extension

An Unreal Engine plugin that extends the Gameplay Tags system with enhanced functionality, including event handling and component-based tag management.

## Features

- **Tag Management**: Easily manage gameplay tags on actors through components
- **Event System**: Built-in event system using gameplay tags
- **Querying**: Find actors and components based on their tags
- **Blueprint Support**: Full Blueprint and C++ support

## Installation

1. Copy the `GameplayTagExtension` folder to your project's `Plugins` directory
2. Enable the plugin in the Unreal Editor (Edit > Plugins > Project > Gameplay Tag Extension)
3. Restart the editor if prompted

## Usage

### Adding Tags to an Actor

1. Add the `UnifyGameplayTagsComponent` to your actor
2. Add tags in the Details panel or via Blueprint/C++

### Using Tag Events

```cpp
// Binding to an event
GetWorld()->GetSubsystem<UUnifyGameplayTagsSubsystem>()->BindGameplayTagEvent(
    this, 
    FGameplayTag::RequestGameplayTag("Event.YourEvent"),
    FGameplayTagEventCallback::CreateUObject(this, &YourClass::HandleEvent));

// Triggering an event
GetWorld()->GetSubsystem<UUnifyGameplayTagsSubsystem>()->TriggerGameplayTagEvent(
    this, 
    FGameplayTag::RequestGameplayTag("Event.YourEvent"),
    OptionalPayloadObject);
```

