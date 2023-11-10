#pragma once

#include "systems/SystemInterface.h"
#include <Factories.h>
#include <SDLApp.h>
#include <SDLBackgoundSprite.h>
#include <SDLKeyStateMap.h>
#include <SDLVec.h>
#include <entt/entt.hpp>

#include <memory>

namespace game {

class Game
{
private:
    pg::config::WindowConfig windowConfig{0, {0, 0}, {1024, 768}, "minimal demo"};
    pg::SDLApp               sdlApp{windowConfig};
    pg::KeyStateMap          keyStateMap{sdlApp.getEventHandler()};

    entt::registry   registry;
    entt::dispatcher dispatcher;
    // TODO: those are really systems.Currently this is mixing setup and per-frame action.
    std::vector<std::unique_ptr<SystemInterface>> systems;

    void renderFrame(const FrameStamp& frameStamp);

    void frame(FrameStamp frameStamp);

public:
    entt::registry& getRegistry();
    entt::dispatcher& getDispatcher();

    pg::SDLApp& getApp();

    pg::KeyStateMap& getKeyStateMap();

    void setup();

    void loop();
};
} // namespace game