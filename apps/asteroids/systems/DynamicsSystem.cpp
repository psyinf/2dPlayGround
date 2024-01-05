#include "DynamicsSystem.hpp"
#include "core/Game.h"
#include "Player.h"
#include "entities/Entities.h"

void game::DynamicsSystem::setup() {}

void game::DynamicsSystem::handle(const FrameStamp& frameStamp)
{
    auto& registry = game.getRegistry();

    auto view = game.getRegistry().view<pg::Transform, game::Dynamics>();
    for (auto& entity : view)
    {
        auto&& [transform, dynamics] = view.get<pg::Transform, game::Dynamics>(entity);
        transform.pos += dynamics.velocity * frameStamp.getFrameDuration_sec();
        dynamics.velocity = dynamics.velocity * dynamics.dampening;
    }
}
