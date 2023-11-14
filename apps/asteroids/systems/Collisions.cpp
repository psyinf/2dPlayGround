#include "Collisions.hpp"
#include <SDLBounds.h>
#include <events/Collison.h>
#include <core/Game.h>
#include <entities/Entities.h>

void game::Collisions::setup() {}

void game::Collisions::handle(const FrameStamp& frameStamp)
{
    auto active_view = game.getRegistry().view<pg::Transform, pg::BoundingSphere, game::ActiveCollider>();
    auto passive_view = game.getRegistry().view<pg::Transform, pg::BoundingSphere, game::PassiveCollider>();
   
    for (auto& entity_a : active_view)
    {
        auto&& [transform_a, bs_a] = active_view.get<pg::Transform, pg::BoundingSphere>(entity_a);
        for (auto& entity_b : passive_view) {
            auto&& [transform_b, bs_b] = active_view.get<pg::Transform, pg::BoundingSphere>(entity_b);
            if (pg::intersects(bs_a, transform_a, bs_b, transform_b)) {
                game.getDispatcher().trigger<events::Collision>( {entity_a, entity_b});
            }
        }

    }
}
