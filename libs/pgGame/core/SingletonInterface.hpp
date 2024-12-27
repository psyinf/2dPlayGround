#pragma once
#include <entt/core/fwd.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <stdexcept>
#include <type_traits>
#include <string_view>
#include <pgf/caching/ResourceCache.hpp>
#include <spdlog/spdlog.h>

namespace pg {

// functional getter/setter interfaces
// TODO: for now
template <typename T>
using SetterFunc = std::function<void(T)>;

template <typename T>
using GetterFunc = std::function<const T()>;

template <typename T>
struct Accessor
{
    SetterFunc<T> setterFunc;
    GetterFunc<T> getterFunc;

    void set(T value) { setterFunc(value); }

    const T get() const { return getterFunc(); }
};

// specify access function via template specialization
template <typename T>
class SingletonInterface
{
public:
    virtual ~SingletonInterface() = default;

    template <typename Type>
    bool hasSingleton(std::string_view id)
    {
        return registry().ctx().template contains<Type>(entt::hashed_string{id.data()});
    }

    // TODO: check const-ness
    template <typename Type>
    auto& getSingleton(std::string_view id)
    {
        if (!hasSingleton<Type>(id))
        {
            spdlog::error("Singleton '{}' not found", id);
            throw std::runtime_error(fmt::format("Singleton '{}' not found", id));
        }
        return registry().ctx().template get<Type>(entt::hashed_string{id.data()});
    }

    template <typename Type>
    auto getSingleton_or(std::string_view id, Type&& default_fallback)
    {
        // as long as we cannot prevent this code to have a reference to a temporary in the default_fallback, we need to
        // reject references
        static_assert(!std::is_reference_v<Type>, "default_fallback must not be a reference");
        if (!hasSingleton<Type>(id)) { return default_fallback; }

        return registry().ctx().template get<Type>(entt::hashed_string{id.data()});
    }

    // works only with default constructible types
    template <typename Type>
    auto& getOrCreateSingleton(std::string_view id)
    {
        if (!hasSingleton<Type>(id))
        {
            return registry().ctx().template emplace_as<Type>(entt::hashed_string{id.data()});
        }

        return registry().ctx().template get<Type>(entt::hashed_string{id.data()});
    }

    template <typename Type, typename... Args>
    auto& addSingleton_as(std::string_view id, Args&&... args)
    {
        return registry().ctx().template emplace_as<Type>(entt::hashed_string(id.data()), std::forward<Args>(args)...);
    }

    template <typename Type>
    auto& getSingleton(const entt::id_type id = entt::type_id<Type>().hash())
    {
        if (!registry().ctx().template contains<Type>(id))
        {
            spdlog::error("Singleton '{}' not found", id);
            throw std::runtime_error(fmt::format("Singleton '{}' not found", id));
        }
        return registry().ctx().template get<Type>(id);
    }

    template <typename Type, typename... Args>
    auto& addSingleton_as(const entt::id_type id = entt::type_id<Type>().hash(), Args&&... args)
    {
        return registry().ctx().template emplace_as<Type>(id, std::forward<Args>(args)...);
    }

    template <typename Type, typename... Args>
    auto& addSingleton(Args&&... args)
    {
        return registry().ctx().template emplace_as<Type>(entt::type_id<Type>().hash(), std::forward<Args>(args)...);
    }

    // accessor interface
    template <typename Type>
    void registerAccessor(std::string_view id, SetterFunc<Type>&& setter, GetterFunc<Type>&& getter)
    {
        registry().ctx().template emplace_as<Accessor<Type>>(entt::hashed_string{id.data()},
                                                             Accessor<Type>{std::move(setter), std::move(getter)});
    }

    template <typename Type>
    Type callGetter(std::string_view id)
    {
        auto& accessor = getSingleton<Accessor<Type>>(id);
        return accessor.get();
    }

    template <typename Type>
    void callSetter(std::string_view id, Type value)
    {
        auto& accessor = getSingleton<Accessor<Type>>(id);
        accessor.set(value);
    }

    entt::registry& registry() { return static_cast<T*>(this)->getRegistry(); }
};

} // namespace pg