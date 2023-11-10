#include "Lasers.h"
#include <core/Game.h>
#include <core/RegistryHelper.h>
#include <entities/Entities.h>

void game::Lasers::setup()
{
    game.getDispatcher().sink<game::events::LaserFired>().connect<&Lasers::handleEvent>(this);
}

void game::Lasers::createShot(const events::LaserFired& event)
{
    // TODO: some simple resource manager
    static auto sprite = std::make_shared<pg::Sprite>(
        pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), "../data/laserBlue01.png"));
    Drawable d(sprite);
    //determine shoot position
    auto& shooterTransform = game.getRegistry().get<pg::Transform>(event.shooter);

    game::makeEntity<Drawable, pg::Transform, Dynamics, tag>

        (game.getRegistry(),                                 //
         std::move(d),                                       //
         pg::Transform{.pos{shooterTransform.pos + event.offset}}, //
         {.velocity{0, -4.1}},
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
    }

    std::ranges::for_each(queued, [this](const auto& e) { createShot(e); });

    queued.clear();
}

void game::Lasers::handleEvent(const events::LaserFired& laserFired)
{
    queued.push_back(laserFired);
}
