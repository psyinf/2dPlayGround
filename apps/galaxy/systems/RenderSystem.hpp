#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class RenderSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup();

    void handle(const pg::game::FrameStamp& frameStamp);
};
} // namespace galaxy