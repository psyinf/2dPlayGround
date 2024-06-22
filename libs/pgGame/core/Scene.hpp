#pragma once
#include <pgGame/systems/SystemInterface.hpp>
#include <pgGame/core/FrameStamp.hpp>
#include <pgEngine/math/Transform.hpp>
#include <memory>
#include <vector>
#include <ranges>
#include <entt/entt.hpp>
#include <pgGame/core/SingletonInterface.hpp>

namespace pg::game {
/**
 * A scene is basically a level or a screen in a game and is represented by a collection of entities and systems.
 * Currently the entities are stored in a shared registry and the systems are stored in a vector.
 */
class Scene : public SingletonInterface
{
    // TODO: we probably need to switch/store keystatemap, scene-transform, and modify some global singletons
public:
    static auto constexpr GlobalTransformName = "Scene.globalTransform";
    using Systems = std::vector<std::shared_ptr<SystemInterface>>;

    Scene(Game& game);

    virtual ~Scene() = default;

    const Systems& getSystems() const;

    Systems& getSystems();

    const Transform2D& getGlobalTransform() const { return globalTransform; }

    Transform2D& getGlobalTransform() { return globalTransform; }

    entt::registry& getRegistry() { return registry; }

    pg::game::Game& getGame() { return game_; }

    virtual void start();

    virtual void stop() {}

    void frame(const FrameStamp& frameStamp);

    bool started() const { return started_; }

private:
    Game&       game_;
    Systems     systems_;
    Transform2D globalTransform;
    bool        started_{false};

    entt::registry registry;
};

} // namespace pg::game