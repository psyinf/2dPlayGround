#pragma once
#include <systems/SystemInterface.h>

namespace game {
class DynamicsSystem : public SystemInterface
{
public:
    using SystemInterface::SystemInterface;
    void setup() override;
    void handle(const FrameStamp& frameStamp) override;
};
} // namespace game