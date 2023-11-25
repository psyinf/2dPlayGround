#pragma once
#include "SystemInterface.h"
#include <array>
#include <deque>
#include <entt/entt.hpp>
#include <events/Collison.h>

namespace game {
using entt::literals::operator""_hs;

class Game;

class Asteroids : public SystemInterface
{
    enum class Size
    {
        Tiny,
        Small,
        Medium,
        Large
    };
public:
    using SystemInterface::SystemInterface;

    using tag = entt::tag<"Asteroids"_hs>;

    void setup();

    void createAsteroid(const pg::fVec2& position, const pg::fVec2& velocity, Size size);

    void handle(const FrameStamp& frameStamp);

    void handleEvent(const events::Collision& collision) { collisions.push_back(collision); }

private:

    std::deque<events::Collision> collisions;
};

} // namespace game