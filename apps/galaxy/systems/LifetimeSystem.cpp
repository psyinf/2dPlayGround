#include "LifetimeSystem.hpp"
#include <components/Lifetime.hpp>
#include <pgEngine/math/Random.hpp>

#include <pgGame/core/Game.hpp>
#include <events/DroneEvents.hpp>

void galaxy::LifetimeSystem::setup(std::string_view /*scene_id*/) {}

bool lifeTimeExceeded(const galaxy::Lifetime& lifetime)
{
    auto r = lifetime.reliability_factor; // reliability factor. smaller values mean higher reliability

    auto l = 1.0f - (std::exp(-r * static_cast<float>(lifetime.time_alive / lifetime.expected_lifetime)));

    return (pg::randomBetween(0.0f, 1.0f) < l);
}

void galaxy::LifetimeSystem::handle(const pg::FrameStamp&)
{
    auto view = _game.getGlobalRegistry().view<galaxy::Lifetime>();
    for (auto& entity : view)
    {
        auto& lifetime = view.get<galaxy::Lifetime>(entity);
        // TODO later: take pause into account
        lifetime.time_alive += 1;
        if (lifeTimeExceeded(lifetime))
        {
            _game.getDispatcher().trigger<galaxy::events::DroneFailedEvent>({.entity = entity});
        }
    }
}
