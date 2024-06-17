#include "Player.h"
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/components/WindowDetails.hpp>
#include <pgGame/components/Drawable.hpp>

#include "components/Entities.h"
#include <pgEngine/math/Bounds.hpp>
#include <pgEngine/math/VecOps.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <fmt/format.h>
#include <events/LaserFired.h>

void asteroids::Player::setup()
{
    auto& registry = game.getRegistry();
    auto& keyStateMap = game.getKeyStateMap();

    auto sprite = game.getCurrentScene().getTypedResourceCache<pg::Sprite>().load("playerShip1_blue.png");
    auto windowDetails = game.getCurrentScene().getSingleton<pg::game::WindowDetails>();
    auto player = pg::game::makeEntity<pg::BoundingSphere, pg::game::Drawable, pg::Transform2D, asteroids::Dynamics>(
        game.getRegistry(),
        {.radius = pg::BoundingSphere::fromRectangle(sprite->getDimensions())},                               //
        {sprite},                                                                                             //
        {.pos{windowDetails.windowRect.w * 0.5f, windowDetails.windowRect.h * 0.75f}, .scale = {0.5f, 0.5f}}, //
        {.dampening{0.95f, 0.95f}});

    pg::game::addComponents<playerTag, asteroids::ActiveCollider>(game.getRegistry(), player);

    game.getCurrentScene().addSingleton_as<const entt::entity>("Player", player);
    game.getCurrentScene().addSingleton_as<pg::iVec2>("Player.sprite.size", sprite->getDimensions());

    auto view = game.getRegistry().view<playerTag, pg::Transform2D, asteroids::Dynamics>();
    auto entity = view.front();

    auto&      dynamics = registry.get<asteroids::Dynamics>(entity);
    const auto speed = 20;
    keyStateMap.registerKeyCallback(SDLK_LEFT, [&dynamics, speed](auto) { dynamics.velocity[0] -= speed; });
    keyStateMap.registerKeyCallback(SDLK_RIGHT, [&dynamics, speed](auto) { dynamics.velocity[0] += speed; });
    keyStateMap.registerKeyCallback(SDLK_UP, [&dynamics, speed](auto) { dynamics.velocity[1] -= speed; });
    keyStateMap.registerKeyCallback(SDLK_DOWN, [&dynamics, speed](auto) { dynamics.velocity[1] += speed; });

    auto event = asteroids::events::LaserFired{.offset{}, .shooter = player};
    auto trigger = [event, this](auto) { game.getDispatcher().trigger(event); };
    game.getKeyStateMap().registerKeyCallback(SDLK_SPACE, trigger, true);
}

void asteroids::Player::handle(const pg::game::FrameStamp&)
{
    auto view = game.getRegistry().view<playerTag, pg::Transform2D, asteroids::Dynamics>();
    auto entity = view.front();
    auto&& [transform, dynamics] = view.get<pg::Transform2D, asteroids::Dynamics>(entity);

    const auto windowDetails = game.getCurrentScene().getSingleton<pg::game::WindowDetails>();
    transform.pos[0] =
        static_cast<float>(std::clamp(static_cast<int>(transform.pos[0]), 0, windowDetails.windowRect.w));
    transform.pos[1] =
        static_cast<float>(std::clamp(static_cast<int>(transform.pos[1]), 0, windowDetails.windowRect.h));
    dynamics.velocity = pg::elementWise(std::truncf, dynamics.velocity);
}
