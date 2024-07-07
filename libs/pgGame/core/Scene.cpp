#include <core/Scene.hpp>
#include <core/Game.hpp>
#include <components/WindowDetails.hpp>
#include <systems/SystemsRegistry.hpp>

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

pg::game::Scene::Scene(Game& game, SceneConfig&& cfg)
  : game_(game)
  , _config(std::move(cfg))
{
}

void pg::game::Scene::setup([[maybe_unused]] std::string_view id)
{
    for (const auto& system : _config.systems)
    {
        auto& systems = getSystems();
        systems.emplace_back(pg::game::SystemsFactory::makeSystem(system, getGame()));
    }
}
