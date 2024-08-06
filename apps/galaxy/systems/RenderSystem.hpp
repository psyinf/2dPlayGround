#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

namespace sdl {
class Renderer;
}

namespace pg {
class States;
}

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class RenderSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup();

    void handle(const pg::FrameStamp& frameStamp);

    void drawOverlays();
};
} // namespace galaxy