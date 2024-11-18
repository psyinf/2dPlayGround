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

void asteroids::Player::setup(std::string_view /*scene_id*/)
{
    auto& registry = _game.getGlobalRegistry();

    auto& keyStateMap = _game.getCurrentScene().getKeyStateMap();
    auto  sprite = _game.getResource<pg::Sprite>("playerShip1_blue.png");
    auto  windowDetails = _game.getCurrentScene().getSingleton<pg::game::WindowDetails>();
    auto  player = pg::game::makeEntity<pg::BoundingSphere, pg::game::Drawable, pg::Transform2D, asteroids::Dynamics>(
        _game.getGlobalRegistry(),
        {.radius = pg::BoundingSphere::fromRectangle(sprite->getDimensions())},                               //
        {sprite},                                                                                             //
        {.pos{windowDetails.windowRect.w * 0.5f, windowDetails.windowRect.h * 0.75f}, .scale = {0.5f, 0.5f}}, //
        {.dampening{0.95f, 0.95f}});

    pg::game::addComponents<playerTag, asteroids::ActiveCollider>(_game.getGlobalRegistry(), player);

    _game.getCurrentScene().addSingleton_as<const entt::entity>("Player", player);
    _game.getCurrentScene().addSingleton_as<pg::iVec2>("Player.sprite.size", sprite->getDimensions());

    auto view = _game.getGlobalRegistry().view<playerTag, pg::Transform2D, asteroids::Dynamics>();
    auto entity = view.front();

    auto&      dynamics = registry.get<asteroids::Dynamics>(entity);
    const auto speed = 20;
    keyStateMap.registerKeyCallback(SDLK_LEFT, [&dynamics, speed](auto) { dynamics.velocity[0] -= speed; });
    keyStateMap.registerKeyCallback(SDLK_RIGHT, [&dynamics, speed](auto) { dynamics.velocity[0] += speed; });
    keyStateMap.registerKeyCallback(SDLK_UP, [&dynamics, speed](auto) { dynamics.velocity[1] -= speed; });
    keyStateMap.registerKeyCallback(SDLK_DOWN, [&dynamics, speed](auto) { dynamics.velocity[1] += speed; });
    keyStateMap.registerKeyCallback(SDLK_d, [this](auto) {
        // set renderconfig debug
        auto& renderConfig = _game.getCurrentScene().getSingleton<RenderConfig>();
        renderConfig.renderBroadPhaseCollisionShapes = !renderConfig.renderBroadPhaseCollisionShapes;
    });

    auto event = asteroids::events::LaserFired{.offset{}, .shooter = player};
    auto trigger = [event, this](auto) { _game.getDispatcher().trigger(event); };
    keyStateMap.registerKeyCallback(SDLK_SPACE, trigger, true);
}

void asteroids::Player::handle(const pg::game::FrameStamp&)
{
    auto view = _game.getGlobalRegistry().view<playerTag, pg::Transform2D, asteroids::Dynamics>();
    auto entity = view.front();
    auto&& [transform, dynamics] = view.get<pg::Transform2D, asteroids::Dynamics>(entity);

    const auto windowDetails = _game.getCurrentScene().getSingleton<pg::game::WindowDetails>();
    transform.pos[0] =
        static_cast<float>(std::clamp(static_cast<int>(transform.pos[0]), 0, windowDetails.windowRect.w));
    transform.pos[1] =
        static_cast<float>(std::clamp(static_cast<int>(transform.pos[1]), 0, windowDetails.windowRect.h));
    dynamics.velocity = pg::elementWise(std::truncf, dynamics.velocity);
}
