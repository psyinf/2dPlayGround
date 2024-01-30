#pragma once
#include <systems/SystemInterface.hpp>

namespace game {
class DynamicsSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;
    void setup() override;
    void handle(const pg::game::FrameStamp& frameStamp) override;
};
} // namespace game