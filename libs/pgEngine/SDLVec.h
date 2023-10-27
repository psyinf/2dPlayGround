#pragma once
#include <array>
#include <cstdint>

namespace pg {
template <typename ELEMENT_TYPE>
using Vec2 = std::array<ELEMENT_TYPE, 2>;

template <typename ELEMENT_TYPE>
using Vec3 = std::array<ELEMENT_TYPE, 2>;

using Vec2Window = Vec2<int32_t>;
using iVec2 = Vec2Window;

} // namespace pg