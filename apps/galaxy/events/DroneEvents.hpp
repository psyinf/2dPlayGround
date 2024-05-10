#pragma once

namespace galaxy::events {

struct DroneCreatedEvent
{
    entt::entity entity;
};

struct DroneFailedEvent
{
    entt::entity entity;
};

struct DroneDestroyedEvent
{
    entt::entity entity;
};
} // namespace galaxy::events