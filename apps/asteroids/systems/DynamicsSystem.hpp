#pragma once
#include <pgGame/systems/SystemInterface.hpp>

namespace asteroids {
class DynamicsSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;
    void setup() override;
    void handle(const pg::FrameStamp& frameStamp) override;
};
} // namespace asteroids