#pragma once
#include <pgEngine/primitives/GuiRenderable.hpp>

#include <memory>

namespace pg::game {

static const int32_t DRAWABLE_FIRST = std::numeric_limits<int32_t>::min();
static const int32_t DRAWABLE_LAST = std::numeric_limits<int32_t>::max();

struct Drawable
{
    std::shared_ptr<pg::Renderable> prim;
    int32_t                         order{0};
};

struct GuiDrawable
{
    std::shared_ptr<pg::game::GuiRenderable> prim;
    int32_t                                  order{0};
};
} // namespace pg::game