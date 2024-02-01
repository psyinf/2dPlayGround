#pragma once

#include <factories/Factories.hpp>
#include <core/App.hpp>
#include <sprite/BackgoundSprite.hpp>
#include <core/KeyStateMap.hpp>
#include <core/ResourceCache.hpp>
#include <math/Vec.hpp>
#include <core/FrameStamp.hpp>
#include <systems/SystemInterface.hpp>

#include <entt/entt.hpp>
#include <memory>

namespace pg::game {

class Game
{
public:
    using Systems = std::vector<std::unique_ptr<SystemInterface>>;

private:
    pg::config::WindowConfig windowConfig{0, {0, 0}, {1024, 768}, "minimal demo"}; // TODO: from config
    pg::SDLApp               sdlApp{windowConfig};
    pg::KeyStateMap          keyStateMap{sdlApp.getEventHandler()};
    pg::ResourceCache        resourceCache{"../data/"}; // TODO: from config
    entt::registry           registry;
    entt::dispatcher         dispatcher;

    Systems systems;

    void frame(FrameStamp frameStamp);

public:
    entt::registry& getRegistry();

    entt::dispatcher& getDispatcher();

    pg::SDLApp& getApp();

    pg::KeyStateMap& getKeyStateMap();

    pg::ResourceCache& getResourceCache();

    Systems& getSystems() { return systems; }

    template <typename Resource>
    pg::TypedResourceCache<Resource>& getTypedResourceCache()
    {
        return getSingleton<pg::TypedResourceCache<Resource>>();
    }

    template <typename Type>
    auto& getSingleton(std::string_view id)
    {
        // TODO: find and throw if not found
        return registry.ctx().get<Type>(entt::hashed_string{id.data()});
    }

    template <typename Type, typename... Args>
    auto& addSingleton_as(std::string_view id, Args&&... args)
    {
        return registry.ctx().emplace_as<Type>(entt::hashed_string{id.data()}, std::forward<Args>(args)...);
    }

    template <typename Type>
    auto& getSingleton(const entt::id_type id = entt::type_id<Type>().hash())
    {
        // TODO: find and throw if not found
        return registry.ctx().get<Type>(id);
    }

    template <typename Type, typename... Args>
    auto& addSingleton_as(const entt::id_type id = entt::type_id<Type>().hash(), Args&&... args)
    {
        return registry.ctx().emplace_as<Type>(id, std::forward<Args>(args)...);
    }

    template <typename Type, typename... Args>
    auto& addSingleton(Args&&... args)
    {
        return registry.ctx().emplace_as<Type>(entt::type_id<Type>().hash(), std::forward<Args>(args)...);
    }

    void setup();

    void loop();
};
} // namespace pg::game