#include "Background.h"
#include "core/Game.h"
#include "entities/Entities.h"

void game::Background::handle(const FrameStamp& frameStamp)
{
    auto  view = game.getRegistry().view<backgroundTag, pg::Transform, game::Dynamics>();
    auto& entity = *view.begin();
    auto&& [transform, dynamics] = view.get<pg::Transform, game::Dynamics>(entity);
    transform.pos[1] += dynamics.velocity[1];
}

void game::Background::setup()
{
    auto background = game.getRegistry().create();
    auto& registry = game.getRegistry();
    auto backgroundImg = pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), "../data/spr_stars01.png");
    // auto windowDetails = game.getRegistry().get()
    // TODO add entities as references to the classes
    auto backgroundRect = pg::iVec2{1024,768};
   

    registry.emplace<Drawable>(
        background, std::make_unique<pg::ScrollingSprite>(std::move(backgroundImg), std::move(backgroundRect)));
    registry.emplace<pg::Transform>(background);
    registry.emplace<game::Dynamics>(background, game::Dynamics{.velocity{0,3}});
    registry.emplace<backgroundTag>(background);
}
