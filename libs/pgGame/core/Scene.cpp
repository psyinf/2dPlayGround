#include <core/Scene.hpp>
#include <core/Game.hpp>
#include <components/WindowDetails.hpp>

void pg::game::Scene::frame(FrameStamp& frameStamp)
{
    if (firstFrame_)
    {
        firstFrame_ = false;
        frameStamp.gameTick = 0;
    }
    std::ranges::for_each(systems_, [&frameStamp](auto& system) { system->handle(frameStamp); });
    frameStamp.gameTick++;
}

pg::game::Scene::Systems& pg::game::Scene::getSystems()
{
    return systems_;
}

const pg::game::Scene::Systems& pg::game::Scene::getSystems() const
{
    return systems_;
}

void pg::game::Scene::start()
{
    if (started_)
    {
        spdlog::warn("Scene already started");
        return;
    }
    firstFrame_ = true;
    std::ranges::for_each(getSystems(), [](auto& system) { system->setup(); });
}

pg::game::Scene::Scene(Game& game)
  : game_(game)
{
}
