#include "Background.h"
#include <pgGame/core/Game.hpp>
#include <pgGame/components/WindowDetails.hpp>
#include "components/Entities.h"
#include <pgGame/components/Drawable.hpp>

void asteroids::Background::handle(const pg::game::FrameStamp&)
{
    // TODO: base scrolling speed on the player's velocity
    auto view = _game.getGlobalRegistry().view<pg::Transform2D, asteroids::Dynamics, backgroundTag>();
}

void asteroids::Background::setup(std::string_view scene_id)
{
    auto& registry = _game.getGlobalRegistry();
    auto  background = registry.create();

    auto backgroundImg = pg::SpriteFactory::makeSprite(_game.getApp().getRenderer(), "../data/spr_stars01.png");
    auto windowDetails = _game.getCurrentScene().getSingleton<pg::game::WindowDetails>();
    // TODO add entities as references to the classes
    auto backgroundRect = pg::iVec2{windowDetails.windowRect.w, windowDetails.windowRect.h};

    registry.emplace<pg::game::Drawable>(background, std::make_unique<pg::ScrollingSprite>(std::move(backgroundImg)));
    registry.emplace<pg::Transform2D>(background);
    registry.emplace<asteroids::Dynamics>(background, asteroids::Dynamics{.velocity{0, 200}});
    registry.emplace<backgroundTag>(background);
}
