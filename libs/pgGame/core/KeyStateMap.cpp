#include <core/KeyStateMap.hpp>

namespace pg {

bool operator&(const KeyStateMap::CallbackTrigger& mask, const KeyStateMap::CallbackTrigger& trigger)
{
    if (mask == KeyStateMap::CallbackTrigger::BOTH) { return true; }
    else { return mask == trigger; }
}
} // namespace pg

void pg::KeyStateMap::keyDown(SDL_Keycode keyCode)
{
    pressed[keyCode] = true;
    if (auto hit = directCallbacks.find(keyCode); hit != directCallbacks.end() && hit->second.trigger & CallbackTrigger::PRESSED)
    {
        hit->second.callback(CallbackTrigger::PRESSED);
    }
}

void pg::KeyStateMap::keyUp(SDL_Keycode keyCode)
{
    pressed[keyCode] = false;
    if (auto hit = directCallbacks.find(keyCode);
        hit != directCallbacks.end() && hit->second.trigger & CallbackTrigger::RELEASED)
    {
        hit->second.callback(CallbackTrigger::RELEASED);
    }
}

bool pg::KeyStateMap::isPressed(SDL_Keycode keyCode)
{
    if (auto hit = pressed.find(keyCode); hit != pressed.end()) { return (*hit).second; }
    else { return false; }
}

void pg::KeyStateMap::registerDirectCallback(SDL_Keycode code, DirectCallback&& callback)
{
    directCallbacks.insert_or_assign(code, callback);
}

void pg::KeyStateMap::registerCallback(SDL_Keycode code, Callback&& callback)
{
    callbacks.insert_or_assign(code, callback);
}

void pg::KeyStateMap::evaluateCallbacks() const
{
    for (auto& callback : callbacks)
    {
        if (auto hit = pressed.find(callback.first); hit != pressed.end() && hit->second == true)
        {
            callback.second(callback.first);
        }
    }
}
