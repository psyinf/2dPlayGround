#include "DynamicsSystem.hpp"
#include "Player.h"
#include "components/Entities.h"
#include <pgGame/core/Game.hpp>

void asteroids::DynamicsSystem::setup() {}

void asteroids::DynamicsSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    auto view = game.getRegistry().view<pg::Transform2D, asteroids::Dynamics>();
    for (auto& entity : view)
    {
        auto&& [transform, dynamics] = view.get<pg::Transform2D, asteroids::Dynamics>(entity);
        transform.pos += dynamics.velocity * frameStamp.getFrameDuration_sec();
        dynamics.velocity = dynamics.velocity * dynamics.dampening;
    }
}
