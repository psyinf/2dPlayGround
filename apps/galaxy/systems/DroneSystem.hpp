#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class DroneSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup();

    void makeDrone(pg::fVec2 pos);

    void handle(const pg::game::FrameStamp& frameStamp);
};
} // namespace galaxy