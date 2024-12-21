#pragma once
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/core/FrameStamp.hpp>
#include <pgEngine/math/Transform.hpp>
#include <memory>
#include <vector>
#include <ranges>
#include <entt/entt.hpp>
#include <pgGame/core/SingletonInterface.hpp>
#include <pgGame/systems/SystemsRegistry.hpp>
#include <pgGame/core/KeyStateMap.hpp>

namespace pg::game {

struct SceneConfig
{
    std::string              scene_id;      //< name of the scene
    std::vector<std::string> systems;       //< systems associated with this scene
    std::string              followUpScene; //< scene to switch to after this scene
};

/**
 * A scene is basically a level or a screen in a game and is represented by a collection of entities and systems.
 * Currently the entities are stored in a shared registry and the systems are stored in a vector.
 */
class Scene : public SingletonInterface<Scene>
{
    friend class SingletonInterface<Scene>;
    // TODO: we probably need to switch/store keystatemap, scene-transform, and modify some global singletons
public:
    static auto constexpr GlobalTransformName = "Scene.globalTransform";
    using Systems = std::vector<std::shared_ptr<SystemInterface>>;

    Scene(Game& game, SceneConfig&& cfg);

    void setup(std::string_view id);

    virtual ~Scene() { stop(); }

    virtual void postFrame(FrameStamp&) {};

    const Systems& getSystems() const;

    Systems& getSystems();

    const Transform2D& getGlobalTransform() const { return globalTransform; }

    Transform2D& getGlobalTransform() { return globalTransform; }

    // get the per scene-registry
    entt::registry& getSceneRegistry();

    entt::registry& getGlobalRegistry();

    auto& getDispatcher() { return _dispatcher; }

    pg::game::Game& getGame() { return game_; }

    KeyStateMap& getKeyStateMap() { return *_keyStateMap; }

    virtual void start();

    virtual void stop() {}

    bool started() const { return started_; }

    void frame(FrameStamp& frameStamp);

    const SceneConfig& getSceneConfig() const { return _config; }

protected:
    entt::registry& getRegistry() { return registry; }

private:
    Game&                        game_;
    Systems                      systems_;
    Transform2D                  globalTransform;
    bool                         started_{false};
    bool                         firstFrame_{true};
    std::shared_ptr<KeyStateMap> _keyStateMap;
    SceneConfig                  _config;
    std::string                  _id;

    entt::registry   registry;
    entt::dispatcher _dispatcher;
};

} // namespace pg::game