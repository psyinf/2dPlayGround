#pragma once
#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <stdexcept>
#include <string_view>
#include <pgFoundation/caching/ResourceCache.hpp>
#include <spdlog/spdlog.h>

namespace pg {
// specify access function via template specialization
template <typename T>
class SingletonInterface
{
public:
    virtual ~SingletonInterface() = default;

    // TODO: check const-ness
    template <typename Type>
    auto& getSingleton(std::string_view id)
    {
        if (!registry().ctx().contains<Type>(entt::hashed_string{id.data()}))
        {
            spdlog::error("Singleton '{}' not found", id);
            throw std::runtime_error(fmt::format("Singleton '{}' not found", id));
        }
        return registry().ctx().get<Type>(entt::hashed_string{id.data()});
    }

    template <typename Type>
    auto getSingleton_or(std::string_view id, Type default_fallback)
    {
        if (!registry().ctx().contains<Type>(entt::hashed_string{id.data()})) { return default_fallback; }

        return registry().ctx().get<Type>(entt::hashed_string{id.data()});
    }

    // works only with default constructible types
    template <typename Type>
    auto& getOrCreateSingleton(std::string_view id)
    {
        if (!registry().ctx().contains<Type>(entt::hashed_string{id.data()}))
        {
            return registry().ctx().emplace_as<Type>(entt::hashed_string{id.data()});
        }

        return registry().ctx().get<Type>(entt::hashed_string{id.data()});
    }

    template <typename Type, typename... Args>
    auto& addSingleton_as(std::string_view id, Args&&... args)
    {
        return registry().ctx().emplace_as<Type>(entt::hashed_string{id.data()}, std::forward<Args>(args)...);
    }

    template <typename Type>
    auto& getSingleton(const entt::id_type id = entt::type_id<Type>().hash())
    {
        // TODO: find and throw if not found
        return registry().ctx().get<Type>(id);
    }

    template <typename Type, typename... Args>
    auto& addSingleton_as(const entt::id_type id = entt::type_id<Type>().hash(), Args&&... args)
    {
        return registry().ctx().emplace_as<Type>(id, std::forward<Args>(args)...);
    }

    template <typename Type, typename... Args>
    auto& addSingleton(Args&&... args)
    {
        return registry().ctx().emplace_as<Type>(entt::type_id<Type>().hash(), std::forward<Args>(args)...);
    }

    entt::registry& registry() { return static_cast<T*>(this)->getRegistry(); }
};

} // namespace pg