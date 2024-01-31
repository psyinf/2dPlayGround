#pragma once
#include <entt/entt.hpp>
#include <systems/SystemInterface.hpp>

namespace asteroids {
using entt::literals::operator""_hs;
class Game;

class Background : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    using backgroundTag = entt::tag<"BACKGROUND"_hs>;

    void setup();

    void handle(const pg::game::FrameStamp& frameStamp);
};

} // namespace asteroids