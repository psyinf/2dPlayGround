#pragma once
#include <systems/SystemInterface.hpp>
#include <entt/entt.hpp>

namespace asteroids {
using entt::literals::operator""_hs;

class Game;

class RenderSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;
 
    using playerTag = entt::tag<"PLAYER"_hs>;

    void setup();

    void handle(const pg::game::FrameStamp& frameStamp);
};
} // namespace asteroids