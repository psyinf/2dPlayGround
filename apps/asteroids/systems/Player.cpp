#include "Player.h"
#include "entities/Entities.h"
#include "core/Game.h"
#include <SDLPrimitives.h>
#include <fmt/format.h>

void game::Player::setup()
{
    auto& registry = game.getRegistry();
    auto& keyStateMap = game.getKeyStateMap();
    auto  player = game.getRegistry().create();
    auto& ctx = game.getRegistry().ctx();
    using entt::literals::operator""_hs;
    ctx.emplace_as<const entt::entity>("Player"_hs, player);
    

    auto  sprite = pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), "../data/playerShip1_blue.png");
    ctx.emplace_as<pg::iVec2>("Player.sprite.size"_hs, pg::iVec2{sprite.getTextureRect().w, sprite.getTextureRect().h});
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

void game::Player::handle(const FrameStamp& frameStamp)
{
    auto  view = game.getRegistry().view<playerTag, pg::Transform, game::Dynamics>();
    auto& entity = *view.begin();
    auto&& [transform, dynamics] = view.get<pg::Transform, game::Dynamics>(entity);

    transform.pos[0] += std::lround(dynamics.velocity[0]);
    transform.pos[1] += std::lround(dynamics.velocity[1]);
 
    dynamics.velocity[0] *= dynamics.dampening[0];
    dynamics.velocity[1] *= dynamics.dampening[1];

    
    auto dim = pg::iVec2{1024, 768};
    transform.pos[0] = std::clamp(static_cast<int>(transform.pos[0]), 0, dim[0]);
    transform.pos[1] = std::clamp(static_cast<int>(transform.pos[1]), 0, dim[1]);
    transform.scale = {0.5, 0.5};
}
