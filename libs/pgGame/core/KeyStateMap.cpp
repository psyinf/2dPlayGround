#include <core/KeyStateMap.hpp>

namespace pg {

bool operator&(const KeyStateMap::CallbackTrigger& mask, const KeyStateMap::CallbackTrigger& trigger)
{
    if (mask == KeyStateMap::CallbackTrigger::BOTH) { return true; }
    else { return mask == trigger; }
}
} // namespace pg

bool pg::KeyStateMap::keyEvent(const SDL_KeyboardEvent& event) noexcept
{
    pressed[event.keysym.sym] = {.pressed{event.state == SDL_PRESSED}, .repeated{event.repeat != 0}};

    if (auto hit = directCallbacks.find(event.keysym.sym); hit != directCallbacks.end())
    {
        if (hit->second.trigger & CallbackTrigger::PRESSED) { hit->second.callback(CallbackTrigger::PRESSED); }
        else if (hit->second.trigger & CallbackTrigger::RELEASED) { hit->second.callback(CallbackTrigger::RELEASED); }
        return true;
    }
    return false;
}

bool pg::KeyStateMap::mouseMotion(const SDL_MouseMotionEvent& event) noexcept
{
    if (mouseMovedCallback) { mouseMovedCallback({event.x, event.y}); }
    if (event.state != 0u && mouseDraggedCallback) { mouseDraggedCallback({event.x, event.y}, event.state); }
    if (event.state != 0u && mouseRelativeDraggedCallback)
    {
        mouseRelativeDraggedCallback({event.xrel, event.yrel}, event.state);
    }
    return true;
}

bool pg::KeyStateMap::mouseButtonEvent(const SDL_MouseButtonEvent& buttonEvent) noexcept
{
    if (mousePressedCallback && buttonEvent.clicks == 1)
    {
        mousePressedCallback({buttonEvent.x, buttonEvent.y}, buttonEvent.button, buttonEvent.state == SDL_PRESSED);
        return true;
    }
    else if (mouseDoubleClickedCallback && buttonEvent.clicks == 2)
    {
        mouseDoubleClickedCallback(
            {buttonEvent.x, buttonEvent.y}, buttonEvent.button, buttonEvent.state == SDL_PRESSED);
        return true;
    }
    return false;
}

bool pg::KeyStateMap::mouseWheelEvent(const SDL_MouseWheelEvent& wheelEvent) noexcept
{
    if (mouseWheelCallback)
    {
        mouseWheelCallback({wheelEvent.x, wheelEvent.y});
        return true;
    }
    return false;
}

void pg::KeyStateMap::registerDirectCallback(SDL_Keycode code, DirectCallback&& callback)
{
    directCallbacks.insert_or_assign(code, callback);
}

void pg::KeyStateMap::registerKeyCallback(SDL_Keycode code, Callback&& callback, bool once /* = false*/)
{
    callbacks.insert_or_assign(code, CallbackData{CallbackTrigger::BOTH, callback, once});
}

void pg::KeyStateMap::evaluateCallbacks() const
{
    for (auto&& [key, callback_data] : callbacks)
    {
        if (auto hit = pressed.find(key); hit != pressed.end())
        {
            // copy by value
            const auto keyState = hit->second;
            // TODO: every n-frames
            if (keyState.repeated && callback_data.once)
            { //
                continue;
            }
            if (callback_data.once) //
            {
                pressed[key] = {.pressed{false}, .repeated{keyState.repeated}};
            }

            if (keyState.pressed) { callback_data.callback(key); }
        }
    }
}

void pg::KeyStateMap::reset()
{
    pressed.clear();
    directCallbacks.clear();
    callbacks.clear();
}
