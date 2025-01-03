#include "Collisions.hpp"
#include <pgEngine/math/BoundsOperations.hpp>
#include <pgGame/core/Game.hpp>
#include <components/Entities.h>
#include <events/Collision.h>

void asteroids::Collisions::setup(std::string_view /*scene_id*/) {}

void asteroids::Collisions::handle(const pg::FrameStamp&)
{
    auto active_view = _game.getGlobalRegistry().view<pg::Transform2D, pg::BoundingSphere, asteroids::ActiveCollider>();
    auto passive_view =
        _game.getGlobalRegistry().view<pg::Transform2D, pg::BoundingSphere, asteroids::PassiveCollider>();

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

void asteroids::Collisions::handleCollision(entt::entity id1, entt::entity id2, [[maybe_unused]] float intrusion)
{
    _game.getDispatcher().trigger<events::Collision>({id1, id2});
}
