#include "Game.hpp"
#include <pgEngine/core/FrameStamp.hpp>
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
    _inputEventDispatcher.evaluateCallbacks();

    // update the scene
    scenes.at(currentSceneId)->frame(frameStamp);
}

entt::registry& game::Game::getCurrentSceneRegistry()
{
    if (currentSceneId.empty()) { throw std::invalid_argument("No scene has been set"); }
    return getScene(currentSceneId).getSceneRegistry();
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

    FrameStamp frameStamp{0, 0, sdlApp.getFPSCounter().getLastFrameDuration()};
    while (running)
    {
        frameStamp.frameNumber++;
        frameStamp.lastFrameDuration = sdlApp.getFPSCounter().getLastFrameDuration();
        if (_gameState.pauseState != PauseState::Paused)
        {
            frameStamp.time.add(sdlApp.getFPSCounter().getLastFrameDuration(), _gameState.timeScale);
        }

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
        if (currentSceneId == id) { return getScene(id); }
        std::string idStr{id};
        auto        scene = scenes.at(idStr).get();
        // stop current scene
        if (!currentSceneId.empty())
        {
            auto& currentScene = scenes.at(currentSceneId);
            for (const auto& system : currentScene->getSystems())
            {
                system->exitScene(currentSceneId);
            }

            currentScene->stop();
            _inputEventDispatcher.setHandlerActive(currentSceneId, false);
        }
        currentSceneId = id;
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
