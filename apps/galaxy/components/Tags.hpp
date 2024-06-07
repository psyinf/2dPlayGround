#pragma once
#include <entt/entt.hpp>

namespace pg::tags {
using entt::literals::operator""_hs;

using DebugRenderingItemTag = entt::tag<"RenderDebug"_hs>;
using OverlayRenderingTag = entt::tag<"RenderOverlay"_hs>;
} // namespace pg::tags