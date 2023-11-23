#include "Lasers.h"
#include <core/Game.h>
#include <core/RegistryHelper.h>
#include <entities/Entities.h>

void game::Lasers::setup()
{
    game.getDispatcher().sink<game::events::LaserFired>().connect<&Lasers::handleEvent>(this);
}

struct SpriteResource : public pg::Sprite
{
    SpriteResource(sdl::Renderer& renderer, const std::string& path)
      : pg::Sprite(pg::SpriteFactory::makeSprite(renderer, path))
    {
    }
};

void game::Lasers::createShot(const events::LaserFired& event)
{
    auto sprite = game.getResourceCache().load<pg::Sprite>("../data/laserBlue01.png", [this](const auto& e) {
        return pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), e);
    });

    Drawable d(sprite);
    // determine shoot position
    auto& shooterTransform = game.getRegistry().get<pg::Transform>(event.shooter);

    game::makeEntity<Drawable, pg::Transform, Dynamics, pg::BoundingSphere, tag>

        (game.getRegistry(),                                                          //
         std::move(d),                                                                //
         pg::Transform{.pos{shooterTransform.pos + event.offset}, .scale{0.5, 0.75}}, //
         {.velocity{0, -5.0}},
        //TODO:: smaller or use bounding rect 
         {pg::BoundingSphere::fromRectangle(sprite->getDimensions())},
         {} //

        );
}

void game::Lasers::handle(const FrameStamp& frameStamp)
{
    // TODO: This is should be in a system for updating transforms via dynamics
    auto view = game.getRegistry().view<tag, pg::Transform, game::Dynamics>();
    for (auto& entity : view)
    {
        auto&& [transform, dynamics] = view.get<pg::Transform, game::Dynamics>(entity);
        transform.pos[1] += dynamics.velocity[1];
        // TODO. rather delete in collision handling (e.g. handle collision with upper limit)
        if (transform.pos[1] < 0) { game.getRegistry().destroy(entity); }
    }

    std::ranges::for_each(queued, [this](const auto& e) { createShot(e); });

    queued.clear();
}

void game::Lasers::handleEvent(const events::LaserFired& laserFired)
{
    queued.push_back(laserFired);
}
