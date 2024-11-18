#pragma once
#include <entt/entt.hpp>

namespace pg::tags {
using entt::literals::operator""_hs;
using MarkerTag = entt::tag<"Marker"_hs>;
using DebugRenderingItemTag = entt::tag<"RenderDebug"_hs>;
using GalaxyRenderTag = entt::tag<"GalaxyRenderTag"_hs>;
using SystemRenderTag = entt::tag<"SystemRenderTag"_hs>;
} // namespace pg::tags