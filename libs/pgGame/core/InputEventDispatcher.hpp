#pragma once

#include <sdlpp.hpp>
#include <unordered_map>

#include <ranges>
#include <set>

namespace pg {

class InputEventHandlerInterface
{
public:
    virtual ~InputEventHandlerInterface() = default;
    virtual bool keyEvent(const SDL_KeyboardEvent& event) noexcept = 0;

    virtual bool mouseMotion(const SDL_MouseMotionEvent& event) noexcept = 0;
    virtual bool mouseButtonEvent(const SDL_MouseButtonEvent& event) noexcept = 0;

    virtual bool mouseWheelEvent(const SDL_MouseWheelEvent& event) noexcept = 0;

    virtual void evaluateCallbacks() const = 0;
};

class InputEventDispatcher : public InputEventHandlerInterface
{
public:
    using EventHandlers = std::unordered_map<std::string, std::shared_ptr<InputEventHandlerInterface>>;

    InputEventDispatcher(sdl::EventHandler& eventHandler, EventHandlers&& handlerList)
    {
        eventHandler.keyDown = [this](const SDL_KeyboardEvent& keyboardEvent) { keyEvent(keyboardEvent); };
        eventHandler.keyUp = [this](const SDL_KeyboardEvent& keyboardEvent) { keyEvent(keyboardEvent); };
        eventHandler.mouseMotion = [this](const SDL_MouseMotionEvent& mouseMotionEvent) {
            mouseMotion(mouseMotionEvent);
        };
        eventHandler.mouseButtonDown = [this](const SDL_MouseButtonEvent& buttonEvent) {
            mouseButtonEvent(buttonEvent);
        };
        eventHandler.mouseButtonUp = [this](const SDL_MouseButtonEvent& buttonEvent) { mouseButtonEvent(buttonEvent); };
        eventHandler.mouseWheel = [this](const SDL_MouseWheelEvent& wheelEvent) { mouseWheelEvent(wheelEvent); };

        eventHandlers = std::move(handlerList);
    }

    void registerHandler(const std::string& name, std::shared_ptr<InputEventHandlerInterface> handler)
    {
        eventHandlers.insert_or_assign(name, handler);
    }

    void evaluateCallbacks() const override
    {
        for (auto handler : getFilteredHandlers() | std::views::values)
        {
            handler->evaluateCallbacks();
        }
    }

    void setHandlerActive(const std::string& name, bool active)
    {
        if (!active) { activeHandlers.erase(name); }
        else { activeHandlers.insert(name); }
    }

protected:
    bool keyEvent(const SDL_KeyboardEvent& event) noexcept override
    {
        for (const auto& handler : getFilteredHandlers())
        {
            (handler.second)->keyEvent(event);
        }
        return false;
    }

    bool mouseMotion(const SDL_MouseMotionEvent& event) noexcept override
    {
        // filter out key events if no handler is active

        for (const auto& handler : getFilteredHandlers())
        {
            (handler.second)->mouseMotion(event);
        }
        return false;
    }

    bool mouseButtonEvent(const SDL_MouseButtonEvent& event) noexcept override
    {
        for (auto& handler : getFilteredHandlers())
        {
            (handler.second)->mouseButtonEvent(event);
        }
        return false;
    }

    bool mouseWheelEvent(const SDL_MouseWheelEvent& event) noexcept override
    {
        for (auto& handler : getFilteredHandlers())
        {
            (handler.second)->mouseWheelEvent(event);
        }
        return false;
    }

    EventHandlers getFilteredHandlers() const
    {
        // TODO: gcc 14 does not like this
        //         return EventHandlers{eventHandlers | std::views::filter([this](const auto& handler) {
        //                                  return activeHandlers.contains(handler.first);
        //                              })};

        EventHandlers filteredHandlers;
        for (const auto& handler : eventHandlers)
        {
            if (activeHandlers.contains(handler.first)) { filteredHandlers.insert(handler); }
        }
        return filteredHandlers;
    }

private:
    EventHandlers         eventHandlers;
    std::set<std::string> activeHandlers;
};
} // namespace pg
