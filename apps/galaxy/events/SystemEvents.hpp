#pragma once
#include <entt/entt.hpp>
#include <pgFoundation/NamedTypeRegistrar.hpp>
#include <components/StarSystem.hpp>

namespace galaxy::events {

struct SystemOwnerChangedEvent : pgf::TypeRegistrar<SystemOwnerChangedEvent, "SystemOwnerChangedEvent">
{
    entt::entity               system_entity;
    std::string                previous_owner;
    std::string                owner_faction;
    galaxy::ColonizationStatus status;
    galaxy::ColonizationStatus old_status;
};

} // namespace galaxy::events