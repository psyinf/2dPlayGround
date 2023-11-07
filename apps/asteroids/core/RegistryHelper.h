#pragma once
#include <entt/entt.hpp>

namespace game {

template <typename Component, typename... Args>
void addComponent(entt::registry& registry, entt::entity id, Component&& t)
{
    registry.emplace<Component>(id, std::forward<Component>(t));
}

template <typename Component, typename... Args>
void addComponent(entt::registry& registry, entt::entity id, Component&& t, Args&&... args)
{
    registry.emplace<Component>(id, std::forward<Component>(t));
    addComponent(registry, id, std::forward<Args>(args)...);
}

template <typename... Components>
auto makeEntity(entt::registry& registry, Components&&... components)
{
    auto id = registry.create();
    addComponent(registry, id, std::forward<Components>(components)...);
    return id;
}
} // namespace game