#include "DynamicsSystem.hpp"
#include "Player.h"
#include "entities/Entities.h"
#include <pgGame/core/Game.hpp>

void asteroids::DynamicsSystem::setup() {}

void asteroids::DynamicsSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    auto& registry = game.getRegistry();

    auto view = game.getRegistry().view<pg::Transform, asteroids::Dynamics>();
    for (auto& entity : view)
    {
        auto&& [transform, dynamics] = view.get<pg::Transform, asteroids::Dynamics>(entity);
        transform.pos += dynamics.velocity * frameStamp.getFrameDuration_sec();
        dynamics.velocity = dynamics.velocity * dynamics.dampening;
    }
}
