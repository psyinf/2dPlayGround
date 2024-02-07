#include "Collisions.hpp"
#include <pgEngine/math/BoundsOperations.hpp>
#include <pgGame/core/Game.hpp>
#include <entities/Entities.h>
#include <events/Collison.h>
#include <iostream>

void asteroids::Collisions::setup() {}

void asteroids::Collisions::handle(const pg::game::FrameStamp& frameStamp)
{
    auto active_view = game.getRegistry().view<pg::Transform2D, pg::BoundingSphere, asteroids::ActiveCollider>();
    auto passive_view = game.getRegistry().view<pg::Transform2D, pg::BoundingSphere, asteroids::PassiveCollider>();

    // actives vs passives
    for (auto& entity_a : active_view)
    {
        auto&& [transform_a, bs_a] = active_view.get<pg::Transform2D, pg::BoundingSphere>(entity_a);
        for (auto& entity_b : passive_view)
        {
            auto&& [transform_b, bs_b] = active_view.get<pg::Transform2D, pg::BoundingSphere>(entity_b);
            if (auto intrusion = pg::intersects(bs_a, transform_a, bs_b, transform_b); intrusion.has_value())
            {
                handleCollision(entity_a, entity_b, intrusion.value());
            }
        }
    }
}

void asteroids::Collisions::handleCollision(entt::entity id1, entt::entity id2, float intrusion)
{
    game.getDispatcher().trigger<events::Collision>({id1, id2});
}
