#pragma once

#include <pgEngine/math/Vec.hpp>
#include <string>

namespace pg::config {

struct WindowConfig
{
    uint8_t        screen{};
    pg::Vec2Window offset;
    pg::Vec2Window size;
    std::string    windowName;
};

} // namespace pg::config