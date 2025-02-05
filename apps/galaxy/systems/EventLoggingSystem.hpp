#pragma once
#include <pgGame/systems/SystemInterface.hpp>

namespace galaxy {

class EventLoggingSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view scene_id);

    void handle(const pg::FrameStamp& frameStamp);
};
} // namespace galaxy