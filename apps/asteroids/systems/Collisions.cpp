#include "Collisions.hpp"
#include <SDLBounds.h>

#include <core/Game.h>

void game::Collisions::setup() {}

void game::Collisions::handle(const FrameStamp& frameStamp)
{
    auto view = game.getRegistry().view<pg::Transform, pg::BoundingSphere>();
    for (auto& entity : view)
    {
        auto&& [transform, dynamics] = view.get<pg::Transform, pg::BoundingSphere>(entity);
    }
}
