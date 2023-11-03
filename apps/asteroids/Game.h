#pragma once
#include "Entities.h"
#include <Factories.h>
#include <SDLApp.h>
#include <SDLBackgoundSprite.h>
#include <SDLKeyStateMap.h>
#include <SDLVec.h>
#include <entt/entt.hpp>

#include "Player.h"
#include "Background.h"

#include <memory>

namespace game {

class Game
{
private:
    pg::config::WindowConfig windowConfig{0, {0, 0}, {1024, 768}, "minimal demo"};
    pg::SDLApp               sdlApp{windowConfig};
    pg::KeyStateMap          keyStateMap{sdlApp.getEventHandler()};

    entt::registry registry;

    Player player;
    Background background;

    void renderFrame(const FrameStamp& frameStamp);

    void frame(FrameStamp frameStamp);

  
public:
    entt::registry& getRegistry();;

    pg::SDLApp& getApp();;

    pg::KeyStateMap& getKeyStateMap();;

    void setup();

    void loop();

    Game();
};
} // namespace game