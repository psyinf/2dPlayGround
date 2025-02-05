#include "EventLoggingSystem.hpp"
#include <events/DroneEvents.hpp>
#include <events/SystemEvents.hpp>
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

    static void logSystemOwnerChanged(const galaxy::events::SystemOwnerChangedEvent& system_owner_changed)
    {
        logger->info("System owner changed for system {}({}): {} -> {}, State from {} to {}",
                     systemNameFromEntity(system_owner_changed.system_entity),
                     entt::to_integral(system_owner_changed.system_entity),
                     system_owner_changed.previous_owner,
                     system_owner_changed.owner_faction,
                     magic_enum::enum_name(system_owner_changed.old_status),
                     magic_enum::enum_name(system_owner_changed.status));
    }

    static inline std::shared_ptr<spdlog::logger>          logger{spdlog::stdout_color_mt("GalaxyEventLogger")};
    static inline std::function<std::string(entt::entity)> systemNameFromEntity{};
};

void galaxy::EventLoggingSystem::setup(std::string_view /*scene_id*/)
{
    EventLoggerMonostate::systemNameFromEntity = [this](entt::entity entity) {
        auto& starsystem = _game.getGlobalRegistry().get<galaxy::StarSystemState>(entity);
        return starsystem.name;
    };
    _game.getDispatcher()
        .sink<galaxy::events::FactionActivatedEvent>()
        .connect<&EventLoggerMonostate::logFactionActivated>();

    _game.getDispatcher().sink<galaxy::events::DroneCreatedEvent>().connect<&EventLoggerMonostate::logDroneCreated>();

    _game.getDispatcher().sink<galaxy::events::DroneFailedEvent>().connect<&EventLoggerMonostate::logDroneFailed>();

    _game.getDispatcher()
        .sink<galaxy::events::DroneDestroyedEvent>()
        .connect<&EventLoggerMonostate::logDroneDestroyed>();

    _game.getDispatcher()
        .sink<galaxy::events::SystemOwnerChangedEvent>()
        .connect<&EventLoggerMonostate::logSystemOwnerChanged>();
}

void galaxy::EventLoggingSystem::handle(const pg::FrameStamp& /*frameStamp*/) {}
