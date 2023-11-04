#pragma once
#include "SDLPrimitives.h"
#include <entt/entt.hpp>

namespace game {
using entt::literals::operator""_hs;
class Game;

class Background
{
public:
    using backgroundTag = entt::tag<"BACKGROUND"_hs>;
    Background(Game& game);

    void setup();

    void handle();

private:
    Game& game;
};

} // namespace game