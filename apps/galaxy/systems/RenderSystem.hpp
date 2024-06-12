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

    void handle(const pg::game::FrameStamp& frameStamp);

    void drawOverlays(sdl::Renderer& renderer, pg::States& rendererStates);
};
} // namespace galaxy