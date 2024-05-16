#pragma once

#include <pgEngine/math/Vec.hpp>

namespace pg {

template <typename A, typename B>
bool intersects(const A& a, const B& b)
{
    return a.intersects(b);
}

} // namespace pg