#pragma once
#include <entt/entt.hpp>
#include <string>

#include <pgFoundation/NamedTypeRegistrar.hpp>

namespace pg::game::events {

struct QuitEvent : pgf::TypeRegistrar<QuitEvent, "QuitEvent">
{
};

struct DestroyEntityEvent : pgf::TypeRegistrar<DestroyEntityEvent, "DestroyEntityEvent">
{
    entt::entity entity;
};

struct PlayPauseEvent : pgf::TypeRegistrar<PlayPauseEvent, "PlayPauseEvent">
{
    enum class State
    {
        Play,
        Pause,
    };

    State state;
};
} // namespace pg::game::events