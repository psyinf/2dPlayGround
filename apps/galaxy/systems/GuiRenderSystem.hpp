#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class GuiRenderSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;
    void setup(std::string_view scene_id);

    void handle(const pg::game::FrameStamp& frameStamp);

private:
};
} // namespace galaxy