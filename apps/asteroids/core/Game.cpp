#include "Game.h"
#include "RegistryHelper.h"
#include "entities/Entities.h"
#include "systems/Asteroids.h"
#include "systems/Background.h"
#include "systems/Collisions.hpp"
#include "systems/Lasers.h"
#include "systems/Player.h"
#include "systems/RenderSystem.hpp"
#include "systems/DynamicsSystem.hpp"

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
    // systems = {{Background{*this}}, {Player{*this}}, {Asteroids{*this}}};
    systems.emplace_back(std::make_unique<Lasers>(*this));
    systems.emplace_back(std::make_unique<Player>(*this));
    systems.emplace_back(std::make_unique<Asteroids>(*this));
    systems.emplace_back(std::make_unique<Background>(*this));
    systems.emplace_back(std::make_unique<Collisions>(*this));
    systems.emplace_back(std::make_unique<RenderSystem>(*this));
    systems.emplace_back(std::make_unique<DynamicsSystem>(*this));
    std::ranges::for_each(systems, [](auto& system) { system->setup(); });
    // retrieve the player id
    auto& ctx = getRegistry().ctx();
    using entt::literals::operator""_hs;
    auto playerId = ctx.get<const entt::entity>("Player"_hs);

    auto event = events::LaserFired{.offset{}, .shooter = playerId};
    auto trigger = [event, this](auto) { dispatcher.trigger(event); };
    keyStateMap.registerDirectCallback(SDLK_SPACE, {pg::KeyStateMap::CallbackTrigger::RELEASED, trigger});

    // TODO: from external config
    registry.ctx().emplace<RenderConfig>(RenderConfig{.renderBroadPhaseCollisionShapes = true});

    
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
