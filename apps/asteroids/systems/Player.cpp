#include "Player.h"
#include "core/Game.h"
#include "core/RegistryHelper.h"
#include "entities/Entities.h"
#include <SDLBounds.h>
#include <SDLPrimitives.h>
#include <fmt/format.h>

void game::Player::setup()
{
    auto& registry = game.getRegistry();
    auto& keyStateMap = game.getKeyStateMap();
    // auto  player = game.getRegistry().create();
    auto& ctx = game.getRegistry().ctx();
    using entt::literals::operator""_hs;
    auto sprite = pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), "../data/playerShip1_blue.png");
    auto player = game::makeEntity<pg::BoundingSphere, Drawable, pg::Transform, game::Dynamics, playerTag, game::ActiveCollider>(
        game.getRegistry(),
        {.radius = pg::BoundingSphere::fromRectangle(sprite.getDimensions())}, //
        {std::make_unique<pg::Sprite>(std::move(sprite))},//
        {.pos{100, 100},.scale = {0.5, 0.5}},//
        {},//
        {},//
        {}
        );
    ctx.emplace_as<pg::iVec2>("Player.sprite.size"_hs, sprite.getDimensions());
    ctx.emplace_as<const entt::entity>("Player"_hs, player);

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
    
}
