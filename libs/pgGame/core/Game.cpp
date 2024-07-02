#include "Game.hpp"
#include "FrameStamp.hpp"
#include <components/WindowDetails.hpp>
#include <systems/SystemInterface.hpp>
#include <core/GameExceptions.hpp>
#include <events/SceneManagementEvents.hpp>
#include <events/GameEvents.hpp>
#include <ranges>

using namespace pg;

template <typename T>
struct ScopedSwitchSceneId
{
    ScopedSwitchSceneId(T& holder, T value)
      : value_holder(holder)
      , previous_value(holder)
    {
        value_holder = value;
    }

    ~ScopedSwitchSceneId() { value_holder = previous_value; }

    T  previous_value;
    T& value_holder;
};

class Pimpl : public pg::game::GamePimpl
{
public:
    using pg::game::GamePimpl::GamePimpl;

    void handleSceneSwitchEvent(const pg::game::events::SwitchSceneEvent& sse)
    {
        // TODO: callbacks?
        try
        {
            game.switchScene(sse.new_scene);
            auto& scene = game.getCurrentScene();
            if (!scene.started()) { scene.start(); }
        }
        catch (pg::game::ResourceNotFoundException&)
        {
            spdlog::error("Scene not found: {}", sse.new_scene);
            // TODO: error event?
        }
    }

    void handleQuitEvent(const pg::game::events::QuitEvent&) { game.quit(); }
};

game::Game::Game()
  : pimpl(std::make_unique<Pimpl>(*this))
{
    gui = std::make_unique<pg::Gui>(getApp());
    // register event handlers
    dispatcher.sink<pg::game::events::SwitchSceneEvent>().connect<&Pimpl::handleSceneSwitchEvent>(
        dynamic_cast<Pimpl&>(*pimpl));
    dispatcher.sink<pg::game::events::QuitEvent>().connect<&Pimpl::handleQuitEvent>(dynamic_cast<Pimpl&>(*pimpl));
    createAndSwitchScene("__default__");
}

void game::Game::frame(FrameStamp& frameStamp)
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
    sdlApp.getEventHandler().quit = [this](auto) { running = false; };

    FrameStamp frameStamp{0, 0, sdlApp.getFPSCounter().getLastFrameDuration()};
    while (running)
    {
        frameStamp.frameNumber++;
        frameStamp.lastFrameDuration = sdlApp.getFPSCounter().getLastFrameDuration();

        frame(frameStamp);
        sdlApp.getFPSCounter().frame();
    }
}

pg::game::Scene& game::Game::getScene(std::string_view id)
{
    return *scenes.at(std::string(id));
}

void game::Game::createSceneInternal(std::string_view id, std::unique_ptr<pg::game::Scene>&& scene)
{
    // internal switch of scene for setup
    {
        ScopedSwitchSceneId switcher(currentSceneId, std::string{id});
        scenes.emplace(std::string{id}, std::move(scene));
        auto scenePtr = scenes.at(std::string(id)).get();
        scenePtr->addSingleton<pg::TypedResourceCache<pg::Sprite>>(
            "../data", [this](const auto& e) { return pg::SpriteFactory::makeSprite(getApp().getRenderer(), e); });
        scenePtr->addSingleton<pg::TypedResourceCache<sdl::Texture>>(
            "../data", [this](const auto& e) { return pg::SpriteFactory::makeTexture(getApp().getRenderer(), e); });
        scenePtr->addSingleton<WindowDetails&>(windowDetails);
        scenePtr->addSingleton_as<pg::Transform2D&>(pg::game::Scene::GlobalTransformName,
                                                    getCurrentScene().getGlobalTransform());
        scenePtr->setup(id);
    }
}

pg::game::Scene& game::Game::switchScene(std::string_view id)
{
    try
    {
        auto scene = scenes.at(std::string(id)).get();
        // stop current scene
        if (!currentSceneId.empty())
        {
            auto& currentScene = scenes.at(currentSceneId);
            for (const auto& system : currentScene->getSystems())
            {
                system->exitScene(currentSceneId);
            }

            currentScene->stop();
        }

        for (const auto& system : scene->getSystems())
        {
            system->enterScene(id);
        }

        currentSceneId = id;
        return *scene;
    }

    catch (std::out_of_range& e)
    {
        spdlog::error(fmt::format("Cannot switch scene to {}, not found: {}", id, e.what()));
        throw e;
    }
}

pg::Gui& game::Game::getGui()
{
    return *gui;
}

void game::Game::quit()
{
    running = false;
}