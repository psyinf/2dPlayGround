#include <core/Scene.hpp>
#include <core/Game.hpp>
#include <components/WindowDetails.hpp>


void pg::game::Scene::frame(FrameStamp frameStamp)
{
    std::ranges::for_each(systems, [&frameStamp](auto& system) { system->handle(frameStamp); });
}

pg::game::Scene::Systems& pg::game::Scene::getSystems()
{
    return systems;
}

const pg::game::Scene::Systems& pg::game::Scene::getSystems() const
{
    return systems;
}
