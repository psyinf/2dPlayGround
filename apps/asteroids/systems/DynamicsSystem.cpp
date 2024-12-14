#include "DynamicsSystem.hpp"
#include "Player.h"
#include "components/Entities.h"
#include <pgGame/core/Game.hpp>

void asteroids::DynamicsSystem::setup(std::string_view /*scene_id*/) {}

void asteroids::DynamicsSystem::handle(const pg::FrameStamp& frameStamp)
{
    auto view = _game.getGlobalRegistry().view<pg::Transform2D, asteroids::Dynamics>();
    auto time_passed_sec = frameStamp.getRealTimePassed_sec();
    for (auto& entity : view)
    {
        auto&& [transform, dynamics] = view.get<pg::Transform2D, asteroids::Dynamics>(entity);
        transform.pos += dynamics.velocity * time_passed_sec;
        transform.rotation_deg += dynamics.angularVelocity * time_passed_sec;
        dynamics.velocity = dynamics.velocity * dynamics.dampening;
    }
}
