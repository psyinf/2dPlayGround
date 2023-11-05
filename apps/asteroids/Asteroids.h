#pragma once
#include <entt/entt.hpp>
#include "Entities.h"

namespace game {
using entt::literals::operator""_hs;

class Game;

class Asteroids
{
public:
    Asteroids(Game& game);

    using tag = entt::tag<"Asteroids"_hs>;

    void setup();

    void handle(const FrameStamp& frameStamp);

private:
    game::Game& game;
};

} // namespace game