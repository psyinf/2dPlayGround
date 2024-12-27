#include "Game.hpp"
#include <pgEngine/core/FrameStamp.hpp>
#include <components/WindowDetails.hpp>
#include <systems/SystemInterface.hpp>
#include <core/GameExceptions.hpp>
#include <events/SceneManagementEvents.hpp>
#include <events/GameEvents.hpp>
#include <ranges>
#include <components/singletons/RegisteredPreloaders.hpp>

using namespace pg;

template <typename T>
struct ScopedSwitchSceneId
{
    ScopedSwitchSceneId(T& holder, T value)
      : previous_value(holder)
      , value_holder(holder)

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
        try
        {
            // TODO: support for {next} etc.
            _game.switchScene(sse.new_scene);
            auto& scene = _game.getCurrentScene();
            scene.start();
        }
        catch (pg::game::ResourceNotFoundException&)
        {
            spdlog::error("Scene not found: {}", sse.new_scene);
            // TODO: error event?
        }
    }

    void handleQuitEvent(const pg::game::events::QuitEvent&) { _game.quit(); }

    void handleDestroyEntityEvent(const pg::game::events::DestroyEntityEvent& dee)
    {
        _game.getGlobalRegistry().destroy(dee.entity);
    }

    void handlePlayPauseEvent(const pg::game::events::PlayPauseEvent& ppe)
    {
        if (ppe.state == pg::game::events::PlayPauseEvent::State::Pause)
        {
            _gameState.pauseState = pg::game::PauseState::Paused;
        }
        else { _gameState.pauseState = pg::game::PauseState::Running; }
    }

    void handleTimeScaleEvent(const pg::game::events::TimeScaleEvent& tse) { _gameState.timeScale = tse.time_scale; }
};

game::Game::Game()
  : pimpl(std::make_unique<Pimpl>(*this, _gameState))
  , _inputEventDispatcher(sdlApp.getEventHandler(), {})
{
    gui = std::make_unique<pg::Gui>(getApp());
    // register event handlers
    dispatcher.sink<pg::game::events::SwitchSceneEvent>().connect<&Pimpl::handleSceneSwitchEvent>(
        dynamic_cast<Pimpl&>(*pimpl));
    dispatcher.sink<pg::game::events::QuitEvent>().connect<&Pimpl::handleQuitEvent>(dynamic_cast<Pimpl&>(*pimpl));
    dispatcher.sink<pg::game::events::DestroyEntityEvent>().connect<&Pimpl::handleDestroyEntityEvent>(
        dynamic_cast<Pimpl&>(*pimpl));
    dispatcher.sink<pg::game::events::PlayPauseEvent>().connect<&Pimpl::handlePlayPauseEvent>(
        dynamic_cast<Pimpl&>(*pimpl));
    dispatcher.sink<pg::game::events::TimeScaleEvent>().connect<&Pimpl::handleTimeScaleEvent>(
        dynamic_cast<Pimpl&>(*pimpl));
    createAndSwitchScene({"__default__"});
}

void game::Game::frame(FrameStamp& frameStamp)
{
    // dispatcher update
    dispatcher.update();
    // check preconditions
    if (_currentSceneId.empty()) { throw std::invalid_argument("No scene has been set"); }
    // poll all events
    while (sdlApp.getEventHandler().poll()) {};
    // evaluate all callbacks bound to events
    _inputEventDispatcher.evaluateCallbacks();

    // update the scene
    scenes.at(_currentSceneId)->frame(frameStamp);
}

entt::registry& game::Game::getCurrentSceneRegistry()
{
    if (_currentSceneId.empty()) { throw std::invalid_argument("No scene has been set"); }
    return getScene(_currentSceneId).getSceneRegistry();
}

entt::dispatcher& game::Game::getGlobalDispatcher()
{
    return dispatcher;
}

pg::SDLApp& game::Game::getApp()
{
    return sdlApp;
}

void game::Game::loop()
{
    sdlApp.getEventHandler().quit = [this](auto) { running = false; };

    _currentFrameStamp = {0, 0, sdlApp.getFPSCounter().getLastFrameDuration()};
    while (running)
    {
        float effectiveTimeScale = _gameState.timeScale;
        if (_gameState.pauseState == pg::game::PauseState::Paused) { effectiveTimeScale = 0.0f; }
        _currentFrameStamp.update(sdlApp.getFPSCounter().getLastFrameDuration(), effectiveTimeScale);

        frame(_currentFrameStamp);
        sdlApp.getFPSCounter().frame();
    }
}

pg::game::Scene& game::Game::getScene(std::string_view id)
{
    return *scenes.at(std::string(id));
}

void game::Game::createSceneInternal(std::string_view id, std::unique_ptr<pg::game::Scene>&& scene)
{
    if (id.empty()) { throw std::invalid_argument("Scene id cannot be empty"); }
    // internal switch of scene for setup
    {
        ScopedSwitchSceneId switcher(_currentSceneId, std::string{id});
        scenes.emplace(std::string{id}, std::move(scene));
        auto scenePtr = scenes.at(std::string(id)).get();

        scenePtr->addSingleton<pg::foundation::TypedResourceCache<pg::Sprite>>(
            [this](const auto& e) { return pg::SpriteFactory::makeSprite(getApp().getRenderer(), e); });
        scenePtr->addSingleton<pg::foundation::TypedResourceCache<sdl::Texture>>(
            [this](const auto& e) { return pg::SpriteFactory::makeTexture(getApp().getRenderer(), e); });
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
        if (_currentSceneId == id) { return getScene(id); }
        std::string idStr{id};
        auto        scene = scenes.at(idStr).get();
        // stop current scene
        if (!_currentSceneId.empty())
        {
            auto& currentScene = scenes.at(_currentSceneId);
            for (const auto& system : currentScene->getSystems())
            {
                system->exitScene(_currentSceneId);
            }

            currentScene->stop();
            _inputEventDispatcher.setHandlerActive(_currentSceneId, false);
        }
        _currentSceneId = id;
        for (const auto& system : scene->getSystems())
        {
            system->enterScene(id);
        }
        _inputEventDispatcher.setHandlerActive(idStr, true);

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

entt::registry& game::Game::getSceneRegistry(std::string_view id)
{
    return getScene(id).getSceneRegistry();
}

void game::Game::registerGlobalSingletons(std::string_view scene_id, const SceneConfig& cfg)
{
    addSingleton_as<SceneConfig>(scene_id, cfg);
    addSingleton_as<pg::singleton::RegisteredLoaders>(std::string{scene_id} + ".loaders",
                                                      pg::singleton::RegisteredLoaders{});
}
