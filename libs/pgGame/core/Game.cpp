#include "Game.hpp"
#include "FrameStamp.hpp"
#include <components/WindowDetails.hpp>
#include <systems/SystemInterface.hpp>

#include <ranges>

using namespace pg;

template <typename T>
struct SwitchSceneId
{
    SwitchSceneId(T& holder, T value)
      : value_holder(holder)
      , previous_value(holder)
    {
        value_holder = value;
    }

    ~SwitchSceneId() { value_holder = previous_value; }

    T  previous_value;
    T& value_holder;
};

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
    return getScene(currentSceneId).getRegistry();
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
    createAndSwitchScene("__default__");

    // TODO: we either need a global registry for this or we need to pass the registry to the systems
    //  alternatively we have registries per scene (which would make sense for most cases)
    //  additionally there could be an mechanism where we copy certain singleton from the "default" registry to the
    //  scene registries
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

void game::Game::createScene(std::string_view id)
{
    if (scenes.contains(std::string(id))) { throw std::invalid_argument("Scene already exists"); }

    // internal switch of scene for setup
    {
        SwitchSceneId switcher(currentSceneId, std::string{id});

        scenes.emplace(std::string{id}, std::make_unique<pg::game::Scene>());
        auto& scene = scenes.at(std::string{id});
        addSingleton<pg::TypedResourceCache<pg::Sprite>>(
            "../data", [this](const auto& e) { return pg::SpriteFactory::makeSprite(getApp().getRenderer(), e); });
        addSingleton<pg::TypedResourceCache<sdl::Texture>>(
            "../data", [this](const auto& e) { return pg::SpriteFactory::makeTexture(getApp().getRenderer(), e); });
        addSingleton<WindowDetails&>(windowDetails);
        addSingleton_as<pg::Transform2D&>(pg::game::Scene::GlobalTransformName, scene->getGlobalTransform());
    }
}

pg::game::Scene& game::Game::switchScene(std::string_view id)
{
    auto scene = scenes.at(std::string(id)).get();
    currentSceneId = id;
    // setup systems
    // TODO: only first time?
   
    return *scene;
}

pg::game::Scene& game::Game::createAndSwitchScene(std::string_view id)
{
    createScene(id);
    return switchScene(id);
}
