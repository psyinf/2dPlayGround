#pragma once

#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/Gui.hpp>
#include <pgEngine/primitives/BackgoundSprite.hpp>
#include <pgGame/core/KeyStateMap.hpp>
#include <pgGame/core/ResourceCache.hpp>
#include <pgGame/core/FrameStamp.hpp>
#include <pgGame/core/Scene.hpp>
#include <pgGame/systems/SystemInterface.hpp>

#include <pgEngine/math/Vec.hpp>

#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>
#include <pgGame/components/WindowDetails.hpp>

namespace pg::game {

class Game
{
public:
    using Scenes = std::unordered_map<std::string, std::unique_ptr<Scene>>;
    using Systems = Scene::Systems;

private:
    pg::config::WindowConfig windowConfig{0, {0, 0}, {500, 500}, "minimal demo"}; // TODO: from config
    // TODO vec4 from 2 vec2
    WindowDetails windowDetails{
        {windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]}};
    pg::SDLApp        sdlApp{windowConfig};
    pg::KeyStateMap   keyStateMap{sdlApp.getEventHandler()};
    pg::ResourceCache resourceCache{"../data/"}; // TODO: from config
    entt::registry    registry;
    entt::dispatcher  dispatcher;

    Scenes scenes;

    void frame(FrameStamp frameStamp);

    std::string currentSceneId;

public:
    Game();

    entt::registry& getRegistry();

    entt::dispatcher& getDispatcher();

    pg::SDLApp& getApp();

    pg::KeyStateMap& getKeyStateMap();

    pg::ResourceCache& getResourceCache();

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
        if (!registry.ctx().contains<Type>(entt::hashed_string{id.data()}))
        {
            throw std::runtime_error("Singleton not found");
        }

        return registry.ctx().get<Type>(entt::hashed_string{id.data()});
    }

    template <typename Type>
    auto getSingleton_or(std::string_view id, Type default_fallback)
    {
        if (!registry.ctx().contains<Type>(entt::hashed_string{id.data()})) { return default_fallback; }

        return registry.ctx().get<Type>(entt::hashed_string{id.data()});
    }

    // works only with default constructible types
    template <typename Type>
    auto& getOrCreateSingleton(std::string_view id)
    {
        if (!registry.ctx().contains<Type>(entt::hashed_string{id.data()}))
        {
            return registry.ctx().emplace_as<Type>(entt::hashed_string{id.data()});
        }

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

    /// Scene interfaces
    Scene& createScene(std::string_view id);
    Scene& getScene(std::string_view id);
    void   switchScene(std::string_view id);

    void loop();
};
} // namespace pg::game