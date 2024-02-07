#include <core/Scene.hpp>
#include <core/Game.hpp>
#include <entities/WindowDetails.hpp>

void pg::game::Scene::setup(Game& game)
{
    game.addSingleton_as<pg::Transform2D&>(GlobalTransformName, globalTransform);
    std::ranges::for_each(systems, [](auto& system) { system->setup(); });
}

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
