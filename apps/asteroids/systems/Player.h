#pragma once
#include <pgGame/core/FrameStamp.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgGame/core/KeyStateMap.hpp>
#include <entt/entt.hpp>

namespace asteroids {
using entt::literals::operator""_hs;

class Player : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    using playerTag = entt::tag<"PLAYER"_hs>;

    void setup();

    void handle(const pg::game::FrameStamp& frameStamp);

private:
};
} // namespace asteroids