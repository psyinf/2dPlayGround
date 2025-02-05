#include "EventLoggingSystem.hpp"
#include <events/DroneEvents.hpp>
#include <pgGame/core/Game.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class EventLoggerMonostate
{
public:
    static void logFactionActivated(const galaxy::events::FactionActivatedEvent& faction_activated_event)
    {
        logger->info("Faction activated: {}", faction_activated_event.faction_name);
    }

    static void logDroneCreated(const galaxy::events::DroneCreatedEvent& drone_created_event)
    {
        logger->info("Drone created: {}", entt::to_integral(drone_created_event.entity));
    }

    static void logDroneFailed(const galaxy::events::DroneFailedEvent& drone_failed_event)
    {
        logger->info("Drone failed: {} ({})", entt::to_integral(drone_failed_event.entity), drone_failed_event.reason);
    }

    static void logDroneDestroyed(const galaxy::events::DroneDestroyedEvent& drone_destroyed_event)
    {
        logger->info(
            "Drone destroyed: {} ({})", entt::to_integral(drone_destroyed_event.entity), drone_destroyed_event.reason);
    }

    static inline std::shared_ptr<spdlog::logger> logger{spdlog::stdout_color_mt("GalaxyEventLogger")};
};

void galaxy::EventLoggingSystem::setup(std::string_view /*scene_id*/)
{
    _game.getDispatcher()
        .sink<galaxy::events::FactionActivatedEvent>()
        .connect<&EventLoggerMonostate::logFactionActivated>();

    _game.getDispatcher().sink<galaxy::events::DroneCreatedEvent>().connect<&EventLoggerMonostate::logDroneCreated>();

    _game.getDispatcher().sink<galaxy::events::DroneFailedEvent>().connect<&EventLoggerMonostate::logDroneFailed>();

    _game.getDispatcher()
        .sink<galaxy::events::DroneDestroyedEvent>()
        .connect<&EventLoggerMonostate::logDroneDestroyed>();
}

void galaxy::EventLoggingSystem::handle(const pg::FrameStamp& /*frameStamp*/) {}
