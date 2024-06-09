#include "Game.hpp"
#include "FrameStamp.hpp"
#include <components/WindowDetails.hpp>
#include <systems/SystemInterface.hpp>

#include <ranges>

using namespace pg;

void game::Game::frame(FrameStamp frameStamp)
{
    // check preconditions
    if (currentSceneId.empty()) { throw std::invalid_argument("No scene has been set"); }
    // poll all events
    while (sdlApp.getEventHandler().poll()) {};
    // evaluate all callbacks bound to events
    keyStateMap.evaluateCallbacks();
    // update the scene
    scenes.at(currentSceneId)->frame(frameStamp);
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

void game::Game::loop()
{
    bool done{};
    sdlApp.getEventHandler().quit = [&done](auto) { done = true; };

    uint64_t frameNumber{};
    while (!done)
    {
        frame({frameNumber++, sdlApp.getFPSCounter().getLastFrameDuration()});
        sdlApp.getFPSCounter().frame();
    }
}

game::Game::Game()
{
    addSingleton<pg::TypedResourceCache<pg::Sprite>>(
        "../data", [this](const auto& e) { return pg::SpriteFactory::makeSprite(getApp().getRenderer(), e); });
    addSingleton<pg::TypedResourceCache<sdl::Texture>>(
        "../data", [this](const auto& e) { return pg::SpriteFactory::makeTexture(getApp().getRenderer(), e); });
    addSingleton<WindowDetails&>(windowDetails);
}

pg::game::Scene& game::Game::getScene(std::string_view id)
{
    return *scenes.at(std::string(id));
}

pg::game::Scene& game::Game::createScene(std::string_view id)
{
    return *scenes.emplace(std::string(id), std::make_unique<pg::game::Scene>()).first->second;
}

void game::Game::switchScene(std::string_view id)
{
    auto scene = scenes.at(std::string(id)).get();
    currentSceneId = id;
    // TODO: we need to figure out how to handle the setup of the scene when switching back and forth
    scene->setup(*this);
}
