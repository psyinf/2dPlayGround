#pragma once
#include "SystemInterface.h"
#include <entt/entt.hpp>
#include <events/LaserFired.h>

namespace game {
using entt::literals::operator""_hs;
#include <events/LaserFired.h>
class Lasers : public SystemInterface
{
public:
    using SystemInterface::SystemInterface;
    using tag = entt::tag<"LASERS"_hs>;
    void setup();

    void createShot(const events::LaserFired& event);

    void handle(const FrameStamp& frameStamp);

    void handleEvent(const events::LaserFired& laserFired);

private:
    std::vector<events::LaserFired> queued;
};

} // namespace game