#pragma once
#include <array>
#include <string>

namespace pg::config {

template <typename ELEMENT_TYPE>
using Vec2 = std::array<ELEMENT_TYPE, 2>;

using Vec2Window = Vec2<int32_t>;

struct WindowConfig
{
    Vec2Window  offset;
    Vec2Window  size;
    std::string windowName;
};

} // namespace pg::config