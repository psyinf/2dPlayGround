#pragma once

#include <SDL.h>
#include <unordered_map>
#include <functional>
#include <sdlpp.hpp>

namespace pg {
class KeyStateMap
{
public:
    KeyStateMap(sdl::EventHandler& eventHandler)
    {
        eventHandler.keyDown = [this](const SDL_KeyboardEvent& keyboardEvent) { keyDown(keyboardEvent.keysym.sym); };
        eventHandler.keyUp = [this](const SDL_KeyboardEvent& keyboardEvent) { keyUp(keyboardEvent.keysym.sym); };
    }

    enum class CallbackTrigger
    {
        PRESSED,
        RELEASED,
        BOTH,
    };

    struct DirectCallback
    {
        CallbackTrigger                              trigger;
        std::function<void(CallbackTrigger trigger)> callback;
    };

    using Callback = std::function<void(SDL_Keycode)>;

public:
    void keyDown(SDL_Keycode keyCode);

    void keyUp(SDL_Keycode keyCode);

    bool isPressed(SDL_Keycode keyCode);

    void registerDirectCallback(SDL_Keycode code, DirectCallback&& callback);

    void registerCallback(SDL_Keycode code, Callback&& callback);

    void evaluateCallbacks() const;

private:
    std::unordered_map<SDL_Keycode, bool>           pressed;
    std::unordered_map<SDL_Keycode, DirectCallback> directCallbacks;
    std::unordered_map<SDL_Keycode, Callback>       callbacks;
};

} // namespace pg