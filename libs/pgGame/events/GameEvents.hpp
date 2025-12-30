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

struct TimeScaleEvent : pgf::TypeRegistrar<TimeScaleEvent, "TimeScaleEvent">
{
    float time_scale;
};
} // namespace pg::game::events
