#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

namespace asteroids {
using entt::literals::operator""_hs;
class Game;

class Background : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    using backgroundTag = entt::tag<"BACKGROUND"_hs>;

    void setup(std::string_view scene_id);

    void handle(const pg::FrameStamp& frameStamp);
};

} // namespace asteroids
