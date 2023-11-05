#pragma once
#include <entt/entt.hpp>

namespace game {
using entt::literals::operator""_hs;

class Game;

class Player
{
public:
    Player(Game& game);
 
    using playerTag = entt::tag<"PLAYER"_hs>;

    void setup();

    void handle();

private:
    game::Game& game;
};
} // namespace game