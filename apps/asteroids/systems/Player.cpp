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

    auto& ctx = game.getRegistry().ctx();
    using entt::literals::operator""_hs;

    auto sprite = game.getTypedResourceCache<pg::Sprite>().load("playerShip1_blue.png");
    auto windowDetails = registry.ctx().get<WindowDetails>();
    auto player = game::makeEntity<pg::BoundingSphere, Drawable, pg::Transform, game::Dynamics>(
        game.getRegistry(),
        {.radius = pg::BoundingSphere::fromRectangle(sprite->getDimensions())},                               //
        {sprite},                                                                                             //
        {.pos{windowDetails.windowRect.w * 0.5f, windowDetails.windowRect.h * 0.75f}, .scale = {0.5f, 0.5f}}, //
        {.dampening{0.95f, 0.95f}});

    game::addComponents<playerTag, game::ActiveCollider>(game.getRegistry(), player);
    ctx.emplace_as<pg::iVec2>("Player.sprite.size"_hs, sprite->getDimensions());
    ctx.emplace_as<const entt::entity>("Player"_hs, player);
    auto view = game.getRegistry().view<playerTag, pg::Transform, game::Dynamics>();
    for (auto& entity : view)
    {
        auto&      dynamics = registry.get<game::Dynamics>(entity);
        const auto speed = 20;
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

    auto windowDetails = game.getRegistry().ctx().get<WindowDetails>();
    transform.pos[0] = std::clamp(static_cast<int>(transform.pos[0]), 0, windowDetails.windowRect.w);
    transform.pos[1] = std::clamp(static_cast<int>(transform.pos[1]), 0, windowDetails.windowRect.h);
    dynamics.velocity = elementWise(std::truncl, dynamics.velocity);
}
