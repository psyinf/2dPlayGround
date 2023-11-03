#include "Game.h"

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
    background.handle();
    player.handle();
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
    auto details = registry.create ();
    registry.emplace<WindowDetails>(details,
        WindowDetails{windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]});
    background.setup();
    player.setup();
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
  : player(*this)
  , background(*this)
{
}
