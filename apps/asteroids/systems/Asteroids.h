#pragma once
#include <entt/entt.hpp>
#include "SystemInterface.h"

namespace game {
using entt::literals::operator""_hs;

class Game;

class Asteroids : public SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    using tag = entt::tag<"Asteroids"_hs>;

    void setup();

    void handle(const FrameStamp& frameStamp);
};

} // namespace game