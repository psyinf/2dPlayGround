#pragma once

#include <pgGame/systems/SystemInterface.hpp>

namespace galaxy {

class LifetimeSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup() override;

    void handle(const pg::game::FrameStamp& frameStamp) override;
};
} // namespace galaxy