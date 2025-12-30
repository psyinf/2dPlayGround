#pragma once

#include <entt/entt.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgFoundation/NamedTypeRegistrar.hpp>

#include <components/Faction.hpp>

namespace galaxy::events {
struct FactionActivatedEvent : pgf::TypeRegistrar<FactionActivatedEvent, "FactionCreatedEvent">
{
    std::string faction_name;
};

struct DroneCreatedEvent : pgf::TypeRegistrar<DroneCreatedEvent, "DroneCreatedEvent">
{
    entt::entity    entity;
    galaxy::Faction faction;
    pg::Transform2D transform;
};

struct DroneFailedEvent : pgf::TypeRegistrar<DroneFailedEvent, "DroneFailedEvent">
{
    entt::entity    entity;
    galaxy::Faction faction;
    std::string     reason;
};

struct DroneDestroyedEvent : pgf::TypeRegistrar<DroneDestroyedEvent, "DroneDestroyedEvent">
{
    entt::entity    entity;
    galaxy::Faction faction;
    std::string     reason;
};

// NOT YET USED- currently using DroneFailedEvent
//  struct DroneDestroyedEvent
//  {
//      entt::entity entity;
//  };
} // namespace galaxy::events
