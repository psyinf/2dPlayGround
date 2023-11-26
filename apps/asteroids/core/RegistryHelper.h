#pragma once
#include <entt/entt.hpp>

/**
 * @brief Functions to make entity creation with components added easier. 
 * Instead of doing @code 
 * auto entity = registry::create();
 * registry.emplate<X>({paramToX});
 * registry.emplate<Y>({paramToY,param2ToY});
 * the helper allows you to:
 * auto entity = makeEntity<X,Y>({paramToX},{paramToY,param2ToY});
*/
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
void addComponents(entt::registry& registry, entt::entity entity, Components&&... components)
{
    addComponent(registry, entity, std::forward<Components>(components)...);
}
template <typename... Components>
void addComponents(entt::registry& registry, entt::entity entity)
{
    addComponent(registry, entity, std::forward<Components>({})...);
}


template <typename... Components>
auto makeEntity(entt::registry& registry, Components&&... components)
{
    auto id = registry.create();
    addComponent(registry, id, std::forward<Components>(components)...);
    return id;
}

} // namespace game