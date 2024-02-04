#pragma once
#include <pgGame/systems/SystemInterface.hpp>
#include <pgGame/core/FrameStamp.hpp>
#include <memory>
#include <vector>
#include <ranges>

namespace pg::game {
/**
 * A scene is basically a level or a screen in a game and is represented by a collection of entities and systems.
 * Currently the entities are stored in a shared registry and the systems are stored in a vector.
 */
class Scene
{
    // TODO: we probably need to switch/store keystatemap, scene-transform, and modify some global singletons
public:
    using Systems = std::vector<std::shared_ptr<SystemInterface>>;

    Scene() = default;
    virtual ~Scene() = default;

    const Systems& getSystems() const { return systems; }

    Systems& getSystems() { return systems; }

    void frame(FrameStamp frameStamp)
    {
        std::ranges::for_each(systems, [&frameStamp](auto& system) { system->handle(frameStamp); });
    }

    void setup()
    {
        std::ranges::for_each(systems, [](auto& system) { system->setup(); });
    }

private:
    Systems systems;
};

} // namespace pg::game