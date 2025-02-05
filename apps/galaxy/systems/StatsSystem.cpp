#include "StatsSystem.hpp"

#include <pgGame/core/Game.hpp>
#include <components/Faction.hpp>
#include <components/StarSystem.hpp>
#include <iostream>

void galaxy::StatsSystem::setup(std::string_view /*scene_id*/)
{
    _game.getCurrentScene().addSingleton_as<const Stats&>("galaxy.stats", stats);
    _game.getDispatcher().sink<events::DroneFailedEvent>().connect<&StatsSystem::onDroneDestroyed>(this);
    _game.getDispatcher().sink<events::DroneCreatedEvent>().connect<&StatsSystem::onDroneCreated>(this);
}

void galaxy::StatsSystem::handle(const pg::FrameStamp&)
{
    auto& fpsCounter = _game.getApp().getFPSCounter();
    // count number of starsystems
    auto view = _game.getGlobalRegistry().view<galaxy::StarSystemState>();
    stats.galaxyStats.numSystems = view.size();
    stats.frameStats.lastFrameFPS = fpsCounter.getLastFrameFPS();
    if (fpsCounter.getCurrentFrameCount() % 20 == 0)
    {
        auto fpsStats = fpsCounter.getAverageFPSAndReset();
        stats.frameStats.fpsStats = fpsStats;
    }
    if (fpsCounter.getCurrentFrameCount() % 100 == 0 && _game.getGenericConfig().getConfig<bool>("logFPS", false))
    {
        auto fpsStats = stats.frameStats.fpsStats;
        spdlog::info("FPS Avg/Min/Max: {:.2}/{:.2}/{:.2}", fpsStats.minFPS, fpsStats.maxFPS, fpsStats.averageFPS);
    }
}

void galaxy::StatsSystem::onDroneDestroyed(events::DroneFailedEvent event)
{
    auto  faction = _game.getGlobalRegistry().get<galaxy::Faction>(event.entity);
    auto& factionStatsValues = stats.factionStats[faction.name];
    factionStatsValues.numDronesFailed++;
}

void galaxy::StatsSystem::onDroneCreated(events::DroneCreatedEvent event)
{
    auto  faction = _game.getGlobalRegistry().get<galaxy::Faction>(event.entity);
    auto& factionStatsValues = stats.factionStats[faction.name];
    factionStatsValues.numDronesCreated++;
}
