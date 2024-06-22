#pragma once
#include <entt/entt.hpp>

namespace pg::tags {
using entt::literals::operator""_hs;
using MarkerTag = entt::tag<"Marker"_hs>;
using DebugRenderingItemTag = entt::tag<"RenderDebug"_hs>;
} // namespace pg::tags