#include "Background.h"
#include "Game.h"

game::Background::Background(Game& game)
  : game(game)
{
}

void game::Background::handle()
{
    auto  view = game.getRegistry().view<backgroundTag, pg::Transform>();
    auto& entity = *view.begin();
    auto& transform = view.get<pg::Transform>(entity);
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
    registry.emplace<backgroundTag>(background);
}
