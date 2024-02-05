#include <core/Scene.hpp>
#include <core/Game.hpp>
#include <entities/WindowDetails.hpp>

void pg::game::Scene::setup(Game& game)
{
    const auto& window_details = game.getSingleton<pg::game::WindowDetails>();
    const auto  midpoint = pg::fVec2{window_details.windowRect.w / 2.0f, window_details.windowRect.h / 2.0f};
    globalTransform.pos = midpoint;

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
