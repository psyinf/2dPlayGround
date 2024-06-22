#pragma once
#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <stdexcept>
#include <string_view>
#include <pgGame/core/ResourceCache.hpp>
#include <spdlog/spdlog.h>

namespace pg {

class SingletonInterface
{
public:
    virtual entt::registry& getRegistry() = 0;

    /**
     * Get the resource cache for a specific type of resource.
     */
    template <typename Resource>
    pg::TypedResourceCache<Resource>& getTypedResourceCache()
    {
        return getSingleton<pg::TypedResourceCache<Resource>>();
    }

    // TODO: check const-ness
    template <typename Type>
    auto& getSingleton(std::string_view id)
    {
        if (!getRegistry().ctx().contains<Type>(entt::hashed_string{id.data()}))
        {
            spdlog::error("Singleton '{}' not found", id);
            throw std::runtime_error(fmt::format("Singleton '{}' not found", id));
        }

        return getRegistry().ctx().get<Type>(entt::hashed_string{id.data()});
    }

    template <typename Type>
    auto getSingleton_or(std::string_view id, Type default_fallback)
    {
        if (!getRegistry().ctx().contains<Type>(entt::hashed_string{id.data()})) { return default_fallback; }

        return getRegistry().ctx().get<Type>(entt::hashed_string{id.data()});
    }

    // works only with default constructible types
    template <typename Type>
    auto& getOrCreateSingleton(std::string_view id)
    {
        if (!getRegistry().ctx().contains<Type>(entt::hashed_string{id.data()}))
        {
            return getRegistry().ctx().emplace_as<Type>(entt::hashed_string{id.data()});
        }

        return getRegistry().ctx().get<Type>(entt::hashed_string{id.data()});
    }

    template <typename Type, typename... Args>
    auto& addSingleton_as(std::string_view id, Args&&... args)
    {
        return getRegistry().ctx().emplace_as<Type>(entt::hashed_string{id.data()}, std::forward<Args>(args)...);
    }

    template <typename Type>
    auto& getSingleton(const entt::id_type id = entt::type_id<Type>().hash())
    {
        // TODO: find and throw if not found
        return getRegistry().ctx().get<Type>(id);
    }

    template <typename Type, typename... Args>
    auto& addSingleton_as(const entt::id_type id = entt::type_id<Type>().hash(), Args&&... args)
    {
        return getRegistry().ctx().emplace_as<Type>(id, std::forward<Args>(args)...);
    }

    template <typename Type, typename... Args>
    auto& addSingleton(Args&&... args)
    {
        return getRegistry().ctx().emplace_as<Type>(entt::type_id<Type>().hash(), std::forward<Args>(args)...);
    }
};
} // namespace pg