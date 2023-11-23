#pragma once
#include <entt/entt.hpp>
#include "SystemInterface.h"
namespace game {
using entt::literals::operator""_hs;

class Game;

class RenderSystem : public SystemInterface
{
public:
    using SystemInterface::SystemInterface;
 
    using playerTag = entt::tag<"PLAYER"_hs>;

    void setup();

    void handle(const FrameStamp& frameStamp);
};
} // namespace game