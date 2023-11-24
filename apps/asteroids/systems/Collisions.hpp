#pragma once
#include "SystemInterface.h"
#include <entt/entt.hpp>
#include <events/LaserFired.h>

namespace game {
using entt::literals::operator""_hs;


class Collisions : public SystemInterface
{
public:
    using SystemInterface::SystemInterface;
    
    void setup();

    void handle(const FrameStamp& frameStamp);

 
private:
    void handleCollision(entt::entity id1, entt::entity id2, float intrusion);
};

} // namespace game