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
    try
    {
        preFrame(frameStamp);
        std::ranges::for_each(systems_, [&frameStamp](auto& system) { system->handle(frameStamp); });
        frameStamp.gameTick++;
        postFrame(frameStamp);
    }
    catch (const std::exception& e)
    {
        spdlog::error("Exception caught in Scene::frame of {} ({})", _id, e.what());
    }
 
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
    started_ = true;
}

pg::game::Scene::Scene(Game& game, SceneConfig&& cfg)
  : game_(game)
  , _config(std::move(cfg))
  , _keyStateMap(std::make_unique<KeyStateMap>())
{
}

void pg::game::Scene::setup([[maybe_unused]] std::string_view id)
{
    _id = id;
    game_.getInputEventDispatcher().registerHandler(std::string{id}, _keyStateMap);
    for (const auto& system : _config.systems)
    {
        auto& systems = getSystems();
        systems.emplace_back(pg::game::SystemsFactory::makeSystem(system, getGame()));
    }
    // setup systems setting them up with the scene context they are used in
    for (auto& system : getSystems())
    {
        system->setup(id);
    }
}

entt::registry& pg::game::Scene::getSceneRegistry()
{
    return registry;
}

entt::registry& pg::game::Scene::getGlobalRegistry()
{
    return game_.getGlobalRegistry();
}
