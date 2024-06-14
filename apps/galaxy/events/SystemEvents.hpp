#pragma once

namespace galaxy::events {

struct SystemOwnerChangedEvent
{
    entt::entity system_entity;
};

} // namespace galaxy::events