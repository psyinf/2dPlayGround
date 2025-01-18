#pragma once
#include <pgEngine/primitives/GuiRenderable.hpp>

#include <memory>

namespace pg::game {

static constexpr int32_t DRAWABLE_FIRST = std::numeric_limits<int32_t>::min();
static constexpr int32_t DRAWABLE_LAST = std::numeric_limits<int32_t>::max();

static constexpr int32_t DRAWABLE_DOCKING_AREA = -10000;
static constexpr int32_t DRAWABLE_DEFAULT = 0;
static constexpr int32_t DRAWABLE_OVERLAY_MENU = 1'000'000;

struct Drawable
{
    std::shared_ptr<pg::Renderable> prim;
    int32_t                         order{0};
};

struct GuiDrawable
{
    std::shared_ptr<pg::game::GuiRenderable> prim;
    int32_t                                  order{0};
    bool                                     active{true};
};
} // namespace pg::game