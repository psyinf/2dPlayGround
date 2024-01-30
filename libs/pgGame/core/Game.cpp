#include "Game.hpp"
#include "FrameStamp.hpp"
#include <systems/SystemInterface.hpp>
#include <entities/WindowDetails.hpp>

#include <ranges>

//#include "RegistryHelper.h"
//#include "entities/Entities.h"
/*#include "systems/Asteroids.h"
#include "systems/Background.h"
#include "systems/Collisions.hpp"
#include "systems/Lasers.h"
#include "systems/Player.h"
#include "systems/RenderSystem.hpp"
#include "systems/DynamicsSystem.hpp"*/
using namespace pg;

void game::Game::frame(FrameStamp frameStamp)
{
    // poll all events
    while (sdlApp.getEventHandler().poll()) {};
    // evaluate all callbacks bound to events
    keyStateMap.evaluateCallbacks();
    std::ranges::for_each(systems, [&frameStamp](auto& system) { system->handle(frameStamp); });
}

entt::registry& game::Game::getRegistry()
{
    return registry;
}

entt::dispatcher& game::Game::getDispatcher()
{
    return dispatcher;
}

pg::SDLApp& game::Game::getApp()
{
    return sdlApp;
}

pg::KeyStateMap& game::Game::getKeyStateMap()
{
    return keyStateMap;
}

pg::ResourceCache& game::Game::getResourceCache()
{
    return resourceCache;
}

void game::Game::setup()
{
    registry.ctx().emplace<pg::TypedResourceCache<pg::Sprite>>(
        "../data", [this](const auto& e) { return pg::SpriteFactory::makeSprite(getApp().getRenderer(), e); });
    auto details =
        WindowDetails{windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]};
    registry.ctx().emplace<WindowDetails>(details);
    std::ranges::for_each(systems, [](auto& system) { system->setup(); });
}

void game::Game::loop()
{
    bool done{};
    sdlApp.getEventHandler().quit = [&done](auto) { done = true; };

    uint64_t frameNumber{};
    while (!done)
    {
        frame({ frameNumber++, sdlApp.getFPSCounter().getLastFrameDuration()});
    }
}
