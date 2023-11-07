#pragma once
#include "SystemInterface.h"
#include <entt/entt.hpp>

namespace game {
using entt::literals::operator""_hs;
class Game;

class Background : public SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    using backgroundTag = entt::tag<"BACKGROUND"_hs>;


    void setup();

    void handle(const FrameStamp& frameStamp);
};

} // namespace game