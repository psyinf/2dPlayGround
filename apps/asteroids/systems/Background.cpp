#include "Background.h"
#include <pgGame/core/Game.hpp>
#include <pgGame/components/WindowDetails.hpp>
#include "components/Entities.h"
#include <pgGame/components/Drawable.hpp>

void asteroids::Background::handle(const pg::game::FrameStamp&)
{
    // TODO: base scrolling speed on the player's velocity
    auto& registry = game.getRegistry();
    auto  view = registry.view<pg::Transform2D, asteroids::Dynamics, backgroundTag>();
}

void asteroids::Background::setup()
{
    auto  background = game.getRegistry().create();
    auto& registry = game.getRegistry();
    auto  backgroundImg = pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), "../data/spr_stars01.png");
    auto  windowDetails = game.getSingleton<pg::game::WindowDetails>();
    // TODO add entities as references to the classes
    auto backgroundRect = pg::iVec2{windowDetails.windowRect.w, windowDetails.windowRect.h};

    registry.emplace<pg::game::Drawable>(
        background, std::make_unique<pg::ScrollingSprite>(std::move(backgroundImg), std::move(backgroundRect)));
    registry.emplace<pg::Transform2D>(background);
    registry.emplace<asteroids::Dynamics>(background, asteroids::Dynamics{.velocity{0, 200}});
    registry.emplace<backgroundTag>(background);
}
