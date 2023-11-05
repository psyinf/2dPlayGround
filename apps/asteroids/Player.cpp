#include "Player.h"
#include "Entities.h"
#include "Game.h"
#include <SDLPrimitives.h>
#include <fmt/format.h>
game::Player::Player(game::Game& game)
  : game(game)
{
}

void game::Player::setup()
{
    auto& registry = game.getRegistry();
    auto& keyStateMap = game.getKeyStateMap();
    auto  player = game.getRegistry().create();
    auto  sprite = pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), "../data/playerShip1_blue.png");
    registry.emplace<Drawable>(player, std::make_unique<pg::Sprite>(std::move(sprite)));
    registry.emplace<pg::Transform>(player, pg::Transform{.pos{100, 100}});
    registry.emplace<game::Dynamics>(player, game::Dynamics{});
    registry.emplace<playerTag>(player);

    auto view = game.getRegistry().view<playerTag, pg::Transform, game::Dynamics>();
    for (auto& entity : view)
    {
        auto&      dynamics = registry.get<game::Dynamics>(entity);
        const auto speed = 0.4;
        keyStateMap.registerCallback(SDLK_LEFT, [&dynamics, speed](auto) { dynamics.velocity[0] -= speed; });
        keyStateMap.registerCallback(SDLK_RIGHT, [&dynamics, speed](auto) { dynamics.velocity[0] += speed; });
        keyStateMap.registerCallback(SDLK_UP, [&dynamics, speed](auto) { dynamics.velocity[1] -= speed; });
        keyStateMap.registerCallback(SDLK_DOWN, [&dynamics, speed](auto) { dynamics.velocity[1] += speed; });
    }
}

void game::Player::handle()
{
    auto  view = game.getRegistry().view<playerTag, pg::Transform, game::Dynamics>();
    auto& entity = *view.begin();
    auto&& [transform, dynamics] = view.get<pg::Transform, game::Dynamics>(entity);

    auto sprite_size = 100; // TODO config/properties of the sprite

    transform.pos[0] += std::lround(dynamics.velocity[0]);
    transform.pos[1] += std::lround(dynamics.velocity[1]);
    //fmt::print("v {:.2f}, t {}         \r", dynamics.velocity[0], transform.pos[0]);
    dynamics.velocity[0] *= dynamics.dampening[0];
    dynamics.velocity[1] *= dynamics.dampening[1];

    
    auto dim = pg::iVec2{1024, 768};
    transform.pos[0] = std::clamp(static_cast<int>(transform.pos[0]), 0, dim[0] - sprite_size);
    transform.pos[1] = std::clamp(static_cast<int>(transform.pos[1]), 0, dim[1] - sprite_size);
}
