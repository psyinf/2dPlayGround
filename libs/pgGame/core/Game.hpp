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
#include <pgGame/core/SingletonInterface.hpp>

namespace pg::game {

namespace events {

struct SwitchSceneEvent;
}

class Game
{
public:
    using Scenes = std::unordered_map<std::string, std::unique_ptr<Scene>>;
    using Systems = Scene::Systems;

private:
    pg::config::WindowConfig windowConfig{0, {0, 20}, {800, 800}, "minimal demo"}; // TODO: from config
    // TODO vec4 from 2 vec2
    WindowDetails windowDetails{
        {windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]}};
    pg::SDLApp               sdlApp{windowConfig};
    pg::KeyStateMap          keyStateMap{sdlApp.getEventHandler()};
    pg::ResourceCache        resourceCache{"../data/"}; // TODO: from config
    std::unique_ptr<pg::Gui> gui;

    entt::dispatcher dispatcher;

    Scenes scenes;

    std::string currentSceneId{"__default__"};

public:
    Game();

    entt::registry& getRegistry();

    entt::dispatcher& getDispatcher();

    pg::SDLApp& getApp();

    pg::Gui& getGui();

    pg::KeyStateMap& getKeyStateMap();

    pg::ResourceCache& getResourceCache();

    /// Scene interfaces
    //     template <typename Type = pg::game::Scene, typename... Args>
    //     void createScene(std::string_view id, Args&&... args)
    //     {
    //         // static_assert(std::is_base_of_v<pg::game::Scene, Type>, "Type must be derived from pg::game::Scene");
    //         if (scenes.contains(std::string(id))) { throw std::invalid_argument("Scene already exists"); }
    //         createScene(id, std::make_unique<Type>(*this, std::forward<Args>(args)...));
    //     }
    // TODO: scene management interace?
    template <typename Type = pg::game::Scene>
    void createScene(std::string_view id)
    {
        // static_assert(std::is_base_of_v<pg::game::Scene, Type>, "Type must be derived from pg::game::Scene");
        if (scenes.contains(std::string(id))) { throw std::invalid_argument("Scene already exists"); }
        createScene(id, std::make_unique<Type>(*this));
    }

    Scene& getCurrentScene() { return getScene(currentSceneId); }

    Scene& getScene(std::string_view id);
    Scene& switchScene(std::string_view id);

    template <typename Type = pg::game::Scene, typename... Args>
    Scene& createAndSwitchScene(std::string_view id, Args&&... args)
    {
        // compile time switch for empty args
        if constexpr (sizeof...(Args) == 0) { createScene<Type>(id); }
        // else { createScene<Type>(id, std::forward<Args>(args)...); }
        return switchScene(id);
    }

    void loop();

private:
    void frame(FrameStamp frameStamp);

    void handleSceneSwitchEvent(const pg::game::events::SwitchSceneEvent& sse);

    void createScene(std::string_view id, std::unique_ptr<Scene>&& scene);
};
} // namespace pg::game