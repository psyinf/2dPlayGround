#pragma once

#include <entt/entt.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgFoundation/NamedTypeRegistrar.hpp>

namespace galaxy::events {

struct DroneCreatedEvent : pgf::TypeRegistrar<DroneCreatedEvent, "DroneCreatedEvent">
{
    entt::entity    entity;
    pg::Transform2D transform;
};

struct DroneFailedEvent : pgf::TypeRegistrar<DroneFailedEvent, "DroneFailedEvent">
{
    entt::entity entity;
};

// NOT YET USED- currently using DroneFailedEvent
//  struct DroneDestroyedEvent
//  {
//      entt::entity entity;
//  };
} // namespace galaxy::events