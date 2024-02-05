#pragma once

#include <SDL.h>
#include <unordered_map>
#include <functional>
#include <sdlpp.hpp>
#include <pgEngine/math/Vec.hpp>

namespace pg {

enum class MouseButton : uint8_t
{
    LEFT,
    MIDDLE,
    RIGHT,
    X1,
    X2,
};

class KeyStateMap
{
public:
    using MouseButtonsState = uint8_t;

    KeyStateMap(sdl::EventHandler& eventHandler)
    {
        eventHandler.keyDown = [this](const SDL_KeyboardEvent& keyboardEvent) { keyDown(keyboardEvent.keysym.sym); };
        eventHandler.keyUp = [this](const SDL_KeyboardEvent& keyboardEvent) { keyUp(keyboardEvent.keysym.sym); };
        eventHandler.mouseMotion = [this](const SDL_MouseMotionEvent& mouseMotionEvent) {
            mouseMotion(mouseMotionEvent);
        };
        eventHandler.mouseButtonDown = [this](const SDL_MouseButtonEvent& buttonEvent) {
            mouseButtonEvent(buttonEvent);
        };
        eventHandler.mouseButtonUp = [this](const SDL_MouseButtonEvent& buttonEvent) { mouseButtonEvent(buttonEvent); };
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
    using MouseMovedCallback = std::function<void(pg::iVec2)>;
    using MouseDraggedCallback = std::function<void(pg::iVec2, MouseButtonsState)>;
    using MousePressedCallback = std::function<void(pg::iVec2, MouseButtonsState, bool)>;

public:
    void keyDown(SDL_Keycode keyCode);

    void keyUp(SDL_Keycode keyCode);

    void mouseMotion(const SDL_MouseMotionEvent& event)
    {
        if (mouseMovedCallback) { mouseMovedCallback({event.x, event.y}); }
        if (event.state != 0 && mouseDraggedCallback) { mouseDraggedCallback({event.x, event.y}, event.state); }
    }

    void mouseButtonEvent(const SDL_MouseButtonEvent& buttonEvent)
    {
        if (mousePressedCallback)
        {
            mousePressedCallback({buttonEvent.x, buttonEvent.y}, buttonEvent.button, buttonEvent.state == SDL_PRESSED);
        }
    }

    bool isPressed(SDL_Keycode keyCode);
    /**
     * Register a callback that will be issued immediately when the key is pressed or released
     *
     * @param code
     * @param callback
     */
    void registerDirectCallback(SDL_Keycode code, DirectCallback&& callback);

    /**
     * Register a callback that will queued and issued when evaluateCallbacks is called
     *
     * @param code
     * @param callback
     */
    void registerKeyCallback(SDL_Keycode code, Callback&& callback);

    void registerMouseMovedCallback(MouseMovedCallback&& callback) { mouseMovedCallback = callback; }

    void registerMouseDraggedCallback(MouseDraggedCallback&& callback) { mouseDraggedCallback = callback; }

    void registerMousePressedCallback(MousePressedCallback&& callback) { mousePressedCallback = callback; }

    void evaluateCallbacks() const;

private:
    std::unordered_map<SDL_Keycode, bool>           pressed;
    std::unordered_map<SDL_Keycode, DirectCallback> directCallbacks;
    std::unordered_map<SDL_Keycode, Callback>       callbacks;
    pg::iVec2                                       mousePosition{};

    MouseMovedCallback   mouseMovedCallback;
    MouseDraggedCallback mouseDraggedCallback;
    MousePressedCallback mousePressedCallback;
};

} // namespace pg