#include "Background.h"
#include "core/Game.h"
#include "entities/Entities.h"

void game::Background::handle(const FrameStamp& frameStamp)
{
    // TODO: base scrolling speed on the player's velocity
    auto& registry = game.getRegistry();
    auto  view = registry.view<pg::Transform, game::Dynamics, backgroundTag>();
}

void game::Background::setup()
{
    auto  background = game.getRegistry().create();
    auto& registry = game.getRegistry();
    auto  backgroundImg = pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), "../data/spr_stars01.png");
    auto  windowDetails = game.getRegistry().ctx().get<WindowDetails>();
    // TODO add entities as references to the classes
    auto backgroundRect = pg::iVec2{windowDetails.windowRect.w, windowDetails.windowRect.h};

    registry.emplace<Drawable>(
        background, std::make_unique<pg::ScrollingSprite>(std::move(backgroundImg), std::move(backgroundRect)));
    registry.emplace<pg::Transform>(background);
    registry.emplace<game::Dynamics>(background, game::Dynamics{.velocity{0, 200}});
    registry.emplace<backgroundTag>(background);
}
