#pragma once

#include <pgEngine/math/Vec.hpp>
#include <string>

namespace pg::config {

struct WindowConfig
{
    uint8_t        screen{};
    pg::Vec2Window offset{10, 40};
    pg::Vec2Window size{800, 600};
    std::string    windowName{"GameWindow"};
};

} // namespace pg::config