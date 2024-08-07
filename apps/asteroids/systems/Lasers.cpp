#include "Lasers.h"
#include <pgEngine/math/Bounds.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <components/Entities.h>
#include <pgGame/components/Drawable.hpp>
#include <pgEngine/resources/SpriteResource.hpp>

void asteroids::Lasers::setup()
{
    game.getDispatcher().sink<asteroids::events::LaserFired>().connect<&Lasers::handleEvent>(this);
}

void asteroids::Lasers::createShot(const events::LaserFired& event)
{
    auto& renderer = game.getApp().getRenderer();
    auto  sprite = game.getResource<pg::Sprite, sdl::Renderer&>("../data/laserBlue01.png", renderer);

    pg::game::Drawable d(sprite);
    // determine shoot position
    auto& shooterTransform = game.getRegistry().get<pg::Transform2D>(event.shooter);

    pg::game::makeEntity<pg::game::Drawable, pg::Transform2D, Dynamics, pg::BoundingSphere, tag, ActiveCollider>

        (game.getRegistry(),                                                            //
         std::move(d),                                                                  //
         pg::Transform2D{.pos{shooterTransform.pos + event.offset}, .scale{0.5, 0.75}}, //
         {.velocity{0, -300.0}},
         {pg::BoundingSphere::fromRectangle(sprite->getDimensions())},
         {}, //
         {});
}

void asteroids::Lasers::handle(const pg::game::FrameStamp&)
{
    // TODO: This is should be in a system for updating transforms via dynamics
    auto view = game.getRegistry().view<pg::Transform2D, tag>();
    for (auto& entity : view)
    {
        auto&& transform = view.get<pg::Transform2D>(entity);
        // TODO. rather delete in collision handling (e.g. handle collision with upper limit)
        if (transform.pos[1] < 0) { game.getRegistry().destroy(entity); }
    }

    std::ranges::for_each(queued, [this](const auto& e) { createShot(e); });

    queued.clear();
}

void asteroids::Lasers::handleEvent(const events::LaserFired& laserFired)
{
    queued.push_back(laserFired);
}
