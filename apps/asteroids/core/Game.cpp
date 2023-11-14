#include "Game.h"
#include "entities/Entities.h"
#include "systems/Asteroids.h"
#include "systems/Background.h"
#include "systems/Lasers.h"
#include "systems/Player.h"
#include "systems/Collisions.hpp"

void game::Game::renderFrame(const FrameStamp& frameStamp)
{
    auto& renderer = sdlApp.getRenderer();

    // renderer.setDrawColor(0x00, 0x00, 0x00, 0xff);
    renderer.clear();

    auto view = registry.view<Drawable, pg::Transform>();
    for (auto& entity : view)
    {
        auto&& [drawable, transform] = view.get<Drawable, pg::Transform>(entity);
        drawable.prim->draw(renderer, transform);
    }
    renderer.present();
}

void game::Game::frame(FrameStamp frameStamp)
{
    // poll all events
    while (sdlApp.getEventHandler().poll()) {};
    // evaluate all callbacks bound to events
    keyStateMap.evaluateCallbacks();
    std::ranges::for_each(systems, [&frameStamp](auto& system) { system->handle(frameStamp); });
    // todo: move to system
    renderFrame(frameStamp);
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
    auto details = registry.create();
    registry.emplace<WindowDetails>(
        details,
        WindowDetails{windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]});
    // systems = {{Background{*this}}, {Player{*this}}, {Asteroids{*this}}};
    systems.emplace_back(std::make_unique<Lasers>(*this));
    systems.emplace_back(std::make_unique<Player>(*this));
    systems.emplace_back(std::make_unique<Asteroids>(*this));
    systems.emplace_back(std::make_unique<Background>(*this));
    systems.emplace_back(std::make_unique<Collisions>(*this));
    std::ranges::for_each(systems, [](auto& system) { system->setup(); });
    // retrieve the player id
    auto& ctx = getRegistry().ctx();
    using entt::literals::operator""_hs;
    auto playerId = ctx.get<const entt::entity>("Player"_hs);

    auto event = events::LaserFired{.offset{}, .shooter{playerId}};
    auto trigger = [event, this](auto) { dispatcher.trigger(event); };
    keyStateMap.registerDirectCallback(SDLK_SPACE, {pg::KeyStateMap::CallbackTrigger::RELEASED, trigger});
}

void game::Game::loop()
{
    bool done{};
    sdlApp.getEventHandler().quit = [&done](auto) { done = true; };

    uint64_t frameNumber{};
    while (!done)
    {
        auto frameStart = std::chrono::high_resolution_clock::now();
        frame({frameNumber++, std::chrono::milliseconds(16)});
    }
}
