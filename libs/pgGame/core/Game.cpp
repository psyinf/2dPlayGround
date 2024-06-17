#include "Game.hpp"
#include "FrameStamp.hpp"
#include <components/WindowDetails.hpp>
#include <systems/SystemInterface.hpp>
#include <core/GameExceptions.hpp>
#include <events/SceneManagementEvents.hpp>
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

game::Game::Game()
{
    gui = std::make_unique<pg::Gui>(getApp());
    // register event handlers
    dispatcher.sink<pg::game::events::SwitchSceneEvent>().connect<&game::Game::handleSceneSwitchEvent>(*this);

    createAndSwitchScene("__default__");
}

void game::Game::frame(FrameStamp frameStamp)
{
    // dispatcher update
    dispatcher.update();
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

pg::game::Scene& game::Game::getScene(std::string_view id)
{
    return *scenes.at(std::string(id));
}

void game::Game::createScene(std::string_view id, std::unique_ptr<pg::game::Scene>&& scene)
{
    // internal switch of scene for setup
    {
        SwitchSceneId switcher(currentSceneId, std::string{id});
        scenes.emplace(std::string{id}, std::move(scene));
        auto scenePtr = scenes.at(std::string(id)).get();
        scenePtr->addSingleton<pg::TypedResourceCache<pg::Sprite>>(
            "../data", [this](const auto& e) { return pg::SpriteFactory::makeSprite(getApp().getRenderer(), e); });
        scenePtr->addSingleton<pg::TypedResourceCache<sdl::Texture>>(
            "../data", [this](const auto& e) { return pg::SpriteFactory::makeTexture(getApp().getRenderer(), e); });
        scenePtr->addSingleton<WindowDetails&>(windowDetails);
        scenePtr->addSingleton_as<pg::Transform2D&>(pg::game::Scene::GlobalTransformName,
                                                    getCurrentScene().getGlobalTransform());
    }
}

pg::game::Scene& game::Game::switchScene(std::string_view id)
{
    try
    {
        auto scene = scenes.at(std::string(id)).get();
        currentSceneId = id;
        return *scene;
    }

    catch (std::out_of_range& e)
    {
        spdlog::error(fmt::format("Cannot switch scene to {}, not found: {}", id, e.what()));
        throw e;
    }
}

void game::Game::handleSceneSwitchEvent(const pg::game::events::SwitchSceneEvent& sse)
{
    // TODO: callbacks?
    try
    {
        switchScene(sse.new_scene);
        auto& scene = getCurrentScene();
        if (!scene.started()) { scene.start(); }
    }
    catch (pg::game::ResourceNotFoundException&)
    {
        spdlog::error("Scene not found: {}", sse.new_scene);
        // TODO: error event?
    }
}

pg::Gui& game::Game::getGui()
{
    return *gui;
}
