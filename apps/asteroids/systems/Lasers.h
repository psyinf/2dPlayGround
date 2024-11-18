#pragma once
#include <pgGame/systems/SystemInterface.hpp>
#include <entt/entt.hpp>
#include <events/LaserFired.h>

namespace asteroids {
using entt::literals::operator""_hs;

class Lasers : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;
    using tag = entt::tag<"LASERS"_hs>;
    void setup(std::string_view scene_id);

    void createShot(const events::LaserFired& event);

    void handle(const pg::game::FrameStamp& frameStamp);

    void handleEvent(const events::LaserFired& laserFired);

private:
    std::vector<events::LaserFired> queued;
};

} // namespace asteroids