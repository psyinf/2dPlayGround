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

    KeyStateMap(sdl::EventHandler& eventHandler);

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
    using MouseWheelCallback = std::function<void(pg::iVec2)>;

public:
    void keyEvent(const SDL_KeyboardEvent& keyCode);

    void mouseMotion(const SDL_MouseMotionEvent& event);

    void mouseButtonEvent(const SDL_MouseButtonEvent& buttonEvent);

    void mouseWheelEvent(const SDL_MouseWheelEvent& wheelEvent);

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
     * @param once if true, the callback will not report another key press until key is released before again
     */
    void registerKeyCallback(SDL_Keycode code, Callback&& callback, bool once = false);

    void registerMouseMovedCallback(MouseMovedCallback&& callback) { mouseMovedCallback = callback; }

    void registerMouseDraggedCallback(MouseDraggedCallback&& callback) { mouseDraggedCallback = callback; }

    void registerMouseRelativeDraggedCallback(MouseDraggedCallback&& callback)
    {
        mouseRelativeDraggedCallback = callback;
    }

    void registerMousePressedCallback(MousePressedCallback&& callback) { mousePressedCallback = callback; }

    void registerMouseWheelCallback(MouseWheelCallback&& callback) { mouseWheelCallback = callback; }

    void evaluateCallbacks() const;

private:
    struct CallbackData
    {
        CallbackTrigger trigger;
        Callback        callback;
        bool            once{false};
    };

    struct KeyState
    {
        bool pressed{false};
        bool repeated{false};
    };

    mutable std::unordered_map<SDL_Keycode, KeyState> pressed; // pressed and repeated
    std::unordered_map<SDL_Keycode, DirectCallback>   directCallbacks;
    std::unordered_map<SDL_Keycode, CallbackData>     callbacks;
    pg::iVec2                                         mousePosition{};

    MouseMovedCallback   mouseMovedCallback;
    MouseDraggedCallback mouseDraggedCallback;
    MouseDraggedCallback mouseRelativeDraggedCallback;
    MousePressedCallback mousePressedCallback;
    MouseWheelCallback   mouseWheelCallback;
};

} // namespace pg