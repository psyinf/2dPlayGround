#include "Player.h"
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/entities/WindowDetails.hpp>
#include "entities/Entities.h"
#include <pgEngine/math/Bounds.hpp>
#include <pgEngine/primitives/Primitives.hpp>
#include <fmt/format.h>

void asteroids::Player::setup()
{
    auto& registry = game.getRegistry();
    auto& keyStateMap = game.getKeyStateMap();


    auto sprite = game.getTypedResourceCache<pg::Sprite>().load("playerShip1_blue.png");
    auto windowDetails = game.getSingleton<pg::game::WindowDetails>();
    auto player = pg::game::makeEntity<pg::BoundingSphere, Drawable, pg::Transform, asteroids::Dynamics>(
        game.getRegistry(),
        {.radius = pg::BoundingSphere::fromRectangle(sprite->getDimensions())},                               //
        {sprite},                                                                                             //
        {.pos{windowDetails.windowRect.w * 0.5f, windowDetails.windowRect.h * 0.75f}, .scale = {0.5f, 0.5f}}, //
        {.dampening{0.95f, 0.95f}});

    pg::game::addComponents<playerTag, asteroids::ActiveCollider>(game.getRegistry(), player);

    game.addSingleton_as<const entt::entity>("Player", player);
    game.addSingleton_as<pg::iVec2>("Player.sprite.size", sprite->getDimensions());

    auto view = game.getRegistry().view<playerTag, pg::Transform, asteroids::Dynamics>();
    for (auto& entity : view)
    {
        auto&      dynamics = registry.get<asteroids::Dynamics>(entity);
        const auto speed = 20;
        keyStateMap.registerCallback(SDLK_LEFT, [&dynamics, speed](auto) { dynamics.velocity[0] -= speed; });
        keyStateMap.registerCallback(SDLK_RIGHT, [&dynamics, speed](auto) { dynamics.velocity[0] += speed; });
        keyStateMap.registerCallback(SDLK_UP, [&dynamics, speed](auto) { dynamics.velocity[1] -= speed; });
        keyStateMap.registerCallback(SDLK_DOWN, [&dynamics, speed](auto) { dynamics.velocity[1] += speed; });
    }
}

void asteroids::Player::handle(const pg::game::FrameStamp& frameStamp)
{
    auto playerId = game.getSingleton<const entt::entity>("Player");

    auto view = game.getRegistry().view<playerTag, pg::Transform, asteroids::Dynamics>();
    auto entity = view.front();
    auto&& [transform, dynamics] = view.get<pg::Transform, asteroids::Dynamics>(entity);

    const auto windowDetails = game.getSingleton<pg::game::WindowDetails>();
    transform.pos[0] = std::clamp(static_cast<int>(transform.pos[0]), 0, windowDetails.windowRect.w);
    transform.pos[1] = std::clamp(static_cast<int>(transform.pos[1]), 0, windowDetails.windowRect.h);
    dynamics.velocity = elementWise(std::truncl, dynamics.velocity);
}
