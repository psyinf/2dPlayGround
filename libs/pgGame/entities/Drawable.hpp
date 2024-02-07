#pragma once
#include <pgEngine/primitives/Primitives.hpp>

#include <memory>

namespace pg::game {
struct Drawable
{
    std::shared_ptr<pg::Primitive> prim;
};

} // namespace pg::game