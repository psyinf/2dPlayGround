#pragma once
#include <entt/entt.hpp>
#include <pgFoundation/NamedTypeRegistrar.hpp>

namespace galaxy::events {

struct SystemOwnerChangedEvent : pgf::TypeRegistrar<SystemOwnerChangedEvent, "SystemOwnerChangedEvent">
{
    entt::entity system_entity;
};

} // namespace galaxy::events