#pragma once
#include <pgf/caching/ResourceManager.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/Gui.hpp>
#include <pgFoundation/VirtualFS.hpp>
#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/core/FrameStamp.hpp>

#include <pgEngine/resources/SpriteResource.hpp>
#include <pgGame/components/WindowDetails.hpp>

#include <pgGame/core/Scene.hpp>
#include <pgGame/config/GenericConfig.hpp>
#include <pgGame/config/GameConfig.hpp>
#include <pgFoundation/NamedTypeRegistry.hpp>
#include <pgGame/components/GameState.hpp>

#include <pgGame/core/InputEventDispatcher.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <sdlpp.hpp>

#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <type_traits>

// namespace vfspp {
// class VirtualFileSystem;
// } // namespace vfspp

namespace pg::game {

namespace events {

struct SwitchSceneEvent;
}

class GamePimpl
{
public:
    virtual ~GamePimpl() = default;

    GamePimpl(Game& game, GameState& gameState)
      : _gameState(gameState)
      , _game(game)

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
    using ResourceManager = foundation::ResourceManager;
    using VFSPtr = std::shared_ptr<pg::foundation::VirtualFileSystem>;

private:
    pg::game::GameConfig _gameConfig;

    WindowDetails                       _windowDetails;
    pg::SDLApp                          _sdlApp;
    pg::InputEventDispatcher            _inputEventDispatcher;
    ResourceManager                     _resourceManager;
    std::unique_ptr<pg::Gui>            _gui;
    entt::dispatcher                    _dispatcher;
    entt::registry                      _registry;
    Scenes                              _scenes;
    std::string                         _currentSceneId{"__default__"};
    GenericConfig                       _config;
    pgf::NamedTypeRegistry              _eventNameRegistry;
    GameState                           _gameState;
    FrameStamp                          _currentFrameStamp;
    bool                                _running{true};
    VFSPtr                              _vfs;
    std::unique_ptr<GamePimpl>          _pimpl;
    pg::foundation::DataProviderFactory _dataProviderFactory;

public:
    Game(pg::game::GameConfig&& config);
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
        return _resourceManager.load<Type, Args...>(uri, std::forward<Args>(args)...);
    }

    auto& getResourceManager() { return _resourceManager; }

    template <typename Type = pg::game::Scene, typename... Args>
    void createScene(pg::game::SceneConfig&& cfg = {}, Args&&... args)
    {
        auto scene_id = cfg.scene_id;
        static_assert(std::is_base_of_v<pg::game::Scene, Type>, "Type must be derived from pg::game::Scene");

        if (scene_id.empty()) { throw std::invalid_argument("Empty scene id is not allowed"); }
        if (_scenes.contains(scene_id)) { throw std::invalid_argument("Scene already exists"); }

        registerGlobalSingletons(scene_id, cfg);
        createSceneInternal(scene_id, std::make_unique<Type>(*this, std::move(cfg), std::forward<Args>(args)...));
    }

    Scene& getCurrentScene() { return getScene(_currentSceneId); }

    Scene& getScene(std::string_view id);
    Scene& switchScene(std::string_view id);

    template <typename Type = pg::game::Scene>
    Scene& createAndSwitchScene(SceneConfig&& cfg = {})
    {
        auto id = cfg.scene_id;
        createScene<Type>(std::move(cfg));
        return switchScene(id);
    }

    const auto& getCurrentTimeStamp() const { return _currentFrameStamp; }

    const auto& getCurrentGameState() const { return _gameState; }

    void loop();

    void quit();

    const auto& getGenericConfig() const { return _config; }

    auto& getGenericConfig() { return _config; }

    auto& getGameConfig() const { return _gameConfig; }

    const auto& getGameConfig() { return _gameConfig; }

    auto getCurrentSceneId() const { return _currentSceneId; }

    auto getDataProviderFactory() -> pg::foundation::DataProviderFactory&;

private:
    void frame(FrameStamp& frameStamp);

    void createSceneInternal(std::string_view id, std::unique_ptr<Scene>&& scene);

    void registerGlobalSingletons(std::string_view scene_id, const SceneConfig& cfg);

    entt::registry& getRegistry() { return _registry; }
};

// specialization of resource loading, need to be outside the class (explicit specialization in non-namespace scope)

template <>
inline std::shared_ptr<pg::Sprite> Game::getResource(const std::string& uri)
{
    return _resourceManager.load<pg::Sprite, sdl::Renderer&>(uri, getApp().getRenderer());
}

template <>
inline std::shared_ptr<sdl::Texture> Game::getResource(const std::string& uri)
{
    return _resourceManager.load<sdl::Texture, sdl::Renderer&>(uri, getApp().getRenderer());
}
} // namespace pg::game
