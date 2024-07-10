#pragma once
#include <entt/entt.hpp>

namespace pg::game::events {

struct QuitEvent
{
};

struct DestroyEntityEvent
{
    entt::entity entity;
};
} // namespace pg::game::events