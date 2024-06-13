#pragma once
#include <pgGame/systems/SystemInterface.hpp>
#include <pgGame/core/FrameStamp.hpp>
#include <pgEngine/math/Transform.hpp>
#include <memory>
#include <vector>
#include <ranges>
#include <entt/entt.hpp>

namespace pg::game {
/**
 * A scene is basically a level or a screen in a game and is represented by a collection of entities and systems.
 * Currently the entities are stored in a shared registry and the systems are stored in a vector.
 */
class Scene
{
    // TODO: we probably need to switch/store keystatemap, scene-transform, and modify some global singletons
public:
    static auto constexpr GlobalTransformName = "Scene.globalTransform";
    using Systems = std::vector<std::shared_ptr<SystemInterface>>;

    Scene() = default;

    virtual ~Scene() = default;

    const Systems& getSystems() const;

    Systems& getSystems();

    const Transform2D& getGlobalTransform() const { return globalTransform; }

    Transform2D& getGlobalTransform() { return globalTransform; }

    void frame(FrameStamp frameStamp);

    entt::registry& getRegistry() { return registry; }

    void start()
    {
        std::ranges::for_each(getSystems(), [](auto& system) { system->setup(); });
    }

    void stop() {}

private:
    Systems     systems;
    Transform2D globalTransform;
    // States         globalState;
    entt::registry registry;
};

} // namespace pg::game