#pragma once

#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/Gui.hpp>
#include <pgEngine/primitives/BackgoundSprite.hpp>
#include <pgGame/core/KeyStateMap.hpp>
#include <pgf/caching/ResourceManager.hpp>
#include <pgEngine/resources/SpriteResource.hpp>
#include <pgEngine/core/FrameStamp.hpp>
#include <pgGame/core/Scene.hpp>
#include <pgGame/systems/SystemInterface.hpp>

#include <pgEngine/math/Vec.hpp>

#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>
#include <pgGame/components/WindowDetails.hpp>
#include <pgEngine/primitives/Sprite.hpp>
#include <pgGame/config/GenericConfig.hpp>
#include <pgFoundation/NamedTypeRegistry.hpp>
#include <pgGame/components/GameState.hpp>

namespace pg::game {

namespace events {

struct SwitchSceneEvent;
}

class GamePimpl
{
public:
    virtual ~GamePimpl() = default;

    GamePimpl(Game& game, GameState& gameState)
      : _game(game)
      , _gameState(gameState)
    {
    }

    GameState& _gameState;
    Game&      _game;
};

class Game : public SingletonInterface<Game>
{
    friend class SingletonInterface<Game>;
    friend class GamePimpl;
    Game(Game&&) = delete;
    Game(const Game&) = delete;
    Game& operator=(Game&&) = delete;

public:
    using Scenes = std::unordered_map<std::string, std::unique_ptr<Scene>>;
    using Systems = Scene::Systems;
    using ResourceManager = foundation::ResourceManagerMonostate<foundation::IdentityResourceLocator>;

private:
    pg::config::WindowConfig windowConfig{0, {0, 20}, {800, 800}, "Ad astra!"}; // TODO: from config
    // TODO vec4 from 2 vec2
    WindowDetails windowDetails{
        {windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]}};
    pg::SDLApp               sdlApp{windowConfig};
    pg::InputEventDispatcher _inputEventDispatcher;
    ResourceManager          resourceManager;
    std::unique_ptr<pg::Gui> gui;

    entt::dispatcher dispatcher;
    entt::registry   _registry;

    Scenes scenes;

    std::string currentSceneId{"__default__"};

    GenericConfig _config;

    pgf::NamedTypeRegistry _eventNameRegistry;
    GameState              _gameState;

public:
    Game();
    entt::registry& getSceneRegistry(std::string_view id);

    entt::registry& getCurrentSceneRegistry();

    entt::registry& getGlobalRegistry() { return _registry; }

    entt::dispatcher& getGlobalDispatcher();

    entt::dispatcher& getDispatcher() { return getCurrentScene().getDispatcher(); }

    pgf::NamedTypeRegistry& getEventNameRegistry() { return _eventNameRegistry; }

    pg::SDLApp& getApp();

    pg::Gui& getGui();

    pg::InputEventDispatcher& getInputEventDispatcher() { return _inputEventDispatcher; }

    template <typename Type, typename... Args>
    std::shared_ptr<Type> getResource(const std::string& uri, Args&&... args)
    {
        return resourceManager.get().load<Type, Args...>(uri, std::forward<Args>(args)...);
    }

    auto& getResourceManager() { return resourceManager; }

    /// Scene interfaces
    //     template <typename Type = pg::game::Scene, typename... Args>
    //     void createScene(std::string_view id, Args&&... args)
    //     {
    //         // static_assert(std::is_base_of_v<pg::game::Scene, Type>, "Type must be derived from pg::game::Scene");
    //         if (scenes.contains(std::string(id))) { throw std::invalid_argument("Scene already exists"); }
    //         createScene(id, std::make_unique<Type>(*this, std::forward<Args>(args)...));
    //     }
    // TODO: scene management interface?
    template <typename Type = pg::game::Scene>
    void createScene(std::string_view id, pg::game::SceneConfig&& cfg = {})
    {
        // static_assert(std::is_base_of_v<pg::game::Scene, Type>, "Type must be derived from pg::game::Scene");
        if (scenes.contains(std::string(id))) { throw std::invalid_argument("Scene already exists"); }
        createSceneInternal(id, std::make_unique<Type>(*this, std::move(cfg)));
    }

    Scene& getCurrentScene() { return getScene(currentSceneId); }

    Scene& getScene(std::string_view id);
    Scene& switchScene(std::string_view id);

    template <typename Type = pg::game::Scene>
    Scene& createAndSwitchScene(std::string_view id, SceneConfig&& cfg = {})
    {
        createScene<Type>(id, std::move(cfg));
        return switchScene(id);
    }

    void loop();

    void quit();

    const auto& getConfig() const { return _config; }

    auto& getConfig() { return _config; }

private:
    void frame(FrameStamp& frameStamp);

    void createSceneInternal(std::string_view id, std::unique_ptr<Scene>&& scene);

    entt::registry& getRegistry() { return _registry; }

    bool running{true};

    std::unique_ptr<GamePimpl> pimpl;
};

// specialization of resource loading, need to be outside the class (explicit specialization in non-namespace scope)

template <>
inline std::shared_ptr<pg::Sprite> Game::getResource(const std::string& uri)
{
    return resourceManager.get().load<pg::Sprite, sdl::Renderer&>(uri, getApp().getRenderer());
}

template <>
inline std::shared_ptr<sdl::Texture> Game::getResource(const std::string& uri)
{
    return resourceManager.get().load<sdl::Texture, sdl::Renderer&>(uri, getApp().getRenderer());
}
} // namespace pg::game