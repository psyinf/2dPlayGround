#include "Game.h"
#include "systems/Player.h"
#include "systems/Background.h"
#include "systems/Asteroids.h"
#include "entities/Entities.h"
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
    //todo: move to system
    renderFrame(frameStamp);
}

entt::registry& game::Game::getRegistry()
{
    return registry;
}

pg::SDLApp& game::Game::getApp()
{
    return sdlApp;
}

pg::KeyStateMap& game::Game::getKeyStateMap()
{
    return keyStateMap;
}

void game::Game::setup()
{
    auto details = registry.create();
    registry.emplace<WindowDetails>(
        details,
        WindowDetails{windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]});
    //systems = {{Background{*this}}, {Player{*this}}, {Asteroids{*this}}};
    systems.emplace_back(std::make_unique<Player>(*this));
    systems.emplace_back(std::make_unique<Asteroids>(*this));
    systems.emplace_back(std::make_unique<Background>(*this));

    std::ranges::for_each(systems, [](auto& system) { system->setup(); });
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

game::Game::Game()
{
}
