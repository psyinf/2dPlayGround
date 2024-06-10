#pragma once

#include <entt/entt.hpp>
#include <pgEngine/math/Transform.hpp>

namespace galaxy::events {

struct DroneCreatedEvent
{
    entt::entity    entity;
    pg::Transform2D transform;
};

struct DroneFailedEvent
{
    entt::entity entity;
};

// NOT YET USED- currently using DroneFailedEvent
//  struct DroneDestroyedEvent
//  {
//      entt::entity entity;
//  };
} // namespace galaxy::events