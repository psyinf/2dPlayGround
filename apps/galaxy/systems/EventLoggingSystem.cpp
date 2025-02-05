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

    static inline std::shared_ptr<spdlog::logger> logger{spdlog::stdout_color_mt("GalaxyEventLogger")};
};

void galaxy::EventLoggingSystem::setup(std::string_view /*scene_id*/)
{
    _game.getDispatcher()
        .sink<galaxy::events::FactionActivatedEvent>()
        .connect<&EventLoggerMonostate::logFactionActivated>();
}

void galaxy::EventLoggingSystem::handle(const pg::FrameStamp& /*frameStamp*/) {}
