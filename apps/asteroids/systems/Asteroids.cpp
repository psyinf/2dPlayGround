#include "Asteroids.h"

#include "components/Entities.h"

#include <pgEngine/resources/SpriteResource.hpp>

#include <pgGame/events/GameEvents.hpp>
#include <pgEngine/math/Bounds.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgEngine/math/VecUtils.hpp>
#include <pgEngine/primitives/BackgoundSprite.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/components/Drawable.hpp>
#include <algorithm>
#include <random>

#include <systems/Lasers.h>

struct AsteroidsRandomGen
{
    static inline std::random_device              rd{};
    static inline std::normal_distribution<float> speed{10, 200};
    static inline std::uniform_int_distribution   pos{-200, 1024};

    static auto makeInitial()
    {
        auto velocity = pg::fVec2{0, std::fabs(speed(rd))};
        auto position = pg::fVec2{static_cast<float>(pos(rd)), 0};
        return std::make_pair(position, velocity);
    }

    static auto makeFragment(const pg::fVec2& position)
    {
        // todo add velocity components outwards
        // bonus multiple fragments should add up to the same impulse as the original asteroid
        auto velocity = pg::fVec2{0, std::fabs(speed(rd))};
        return std::make_pair(position, velocity);
    }
};

// TODO: use asteroid/asteroid collision to determine if they should split or deflect
//  return an ordered pair of asteroid and weapon entity or nullopt if the collision is not between an asteroid and a
//  weapon
std::optional<std::pair<entt::entity, entt::entity>> getAsteroidWeaponPair(
    pg::game::Game&                     game,
    const asteroids::events::Collision& collision)
{
    auto isAsteroidE1 = game.getRegistry().all_of<asteroids::Asteroids::tag>(collision.c1);
    auto isAsteroidE2 = game.getRegistry().all_of<asteroids::Asteroids::tag>(collision.c2);
    auto isLaserE1 = game.getRegistry().all_of<asteroids::Lasers::tag>(collision.c1);
    auto isLaserE2 = game.getRegistry().all_of<asteroids::Lasers::tag>(collision.c2);
    std::pair<entt::entity, entt::entity> retVal;
    // TODO: we need a different strategy here.
    //  1. we need to get rid of the pair-wise checking (e.g. asteroid vs laser + laser vs asteroid)
    //  quick idea: this could be done by establishing an order over the entity types
    //  2. the combinations should be
    //  ast - ast -> deflection
    //  ast - las -> split
    //  ast - shp -> damage/deflection/depending on energy -> split
    //  if neither are asteroids or lasers
    if (!isAsteroidE1 && !isAsteroidE2) { return std::nullopt; }
    if (!isLaserE1 && !isLaserE2) { return std::nullopt; }
    // if both are asteroids
    if (isAsteroidE1 && isAsteroidE2) { return std::nullopt; }
    // if both are lasers
    if (isLaserE1 && isLaserE2) { return std::nullopt; }

    if (isAsteroidE1) { retVal.first = collision.c1; }
    else { retVal.first = collision.c2; }
    if (isLaserE1) { retVal.second = collision.c1; }
    else { retVal.second = collision.c2; }
    return retVal;
}

void asteroids::Asteroids::setup()
{
    for ([[maybe_unused]] auto _ : std::views::iota(0, 4))
    {
        auto [pos, vel] = AsteroidsRandomGen::makeInitial();
        createAsteroid(pos, vel, Size::Large);
    }
    game.getDispatcher().sink<asteroids::events::Collision>().connect<&Asteroids::handleEvent>(this);
}

void asteroids::Asteroids::createAsteroid(const pg::fVec2& position, const pg::fVec2& velocity, Size size)
{
    struct AstroidConf
    {
        std::string   resource;
        std::uint16_t hitPoints;
        std::uint16_t damage;
    } asteroidConf;

    switch (size)
    {
    case Size::Large: {
        asteroidConf = {"Meteors/meteorBrown_big1.png", 100, 100};
        break;
    }
    case Size::Medium: {
        asteroidConf = {"Meteors/meteorBrown_med1.png", 60, 60};
        break;
    }
    case Size::Small: {
        asteroidConf = {"Meteors/meteorBrown_small1.png", 30, 30};
        break;
    }
    case Size::Tiny: {
        asteroidConf = {"Meteors/meteorBrown_tiny1.png", 15, 15};
        break;
    }
    }

    auto sprite =
        game.getResource<pg::Sprite, sdl::Renderer&>(std::string(asteroidConf.resource), game.getApp().getRenderer());
    auto entity = pg::game::makeEntity<pg::game::Drawable,
                                       pg::Transform2D,
                                       Dynamics,
                                       pg::BoundingSphere,
                                       HitPoints,
                                       Damage,
                                       Size>                           //
        (game.getRegistry(),                                           //
         pg::game::Drawable{sprite},                                   //
         pg::Transform2D{.pos{position}},                              //
         {.velocity{velocity}},                                        //
         {pg::BoundingSphere::fromRectangle(sprite->getDimensions())}, //
         {asteroidConf.hitPoints},                                     //
         {asteroidConf.damage},                                        //
         {std::move(size)}                                             //
        );
    pg::game::addComponents<PassiveCollider, tag>(game.getRegistry(), entity);
}

void asteroids::Asteroids::handle(const pg::FrameStamp&)
{
    std::random_device              rd;
    std::normal_distribution<float> dist(1, 2);
    std::normal_distribution<float> speed(10, 100);
    std::uniform_int_distribution   pos(-200, 1024);

    // TODO: flag entities that should re-appear after entering a screen border and handle them in a separate system
    auto view = game.getRegistry().view<tag, pg::Transform2D, asteroids::Dynamics>();
    for (auto& entity : view)
    {
        auto&& [transform, dynamics] = view.get<pg::Transform2D, asteroids::Dynamics>(entity);

        if (transform.pos[1] >= 1000)
        {
            transform.pos = {static_cast<float>(pos(rd)), -100}; //
            dynamics.velocity[1] = std::fabs(speed(rd));
        }
    }

    // handle events:
    for (auto&& collision : collisions)
    {
        auto collisionPair = getAsteroidWeaponPair(game, collision);
        if (!collisionPair.has_value()) { continue; }
        auto&& [asteroid, laser] = collisionPair.value();

        // TODO: make based on damage vs hitpoints
        auto&& [transform, dynamics, size] =
            game.getRegistry().get<pg::Transform2D, asteroids::Dynamics, Size>(asteroid);

        auto newSize = getNextSmallest(size);
        if (newSize.has_value())
        {
            auto fragments = pg::splitVector(dynamics.velocity, pg::randomBetween(2, 3));
            for (const auto& fragment : fragments)
            {
                createAsteroid(transform.pos, fragment, newSize.value());
            }
        }
        // trigger explosion
        createExplosion(transform.pos);
        game.getRegistry().destroy(asteroid);
        game.getRegistry().destroy(laser);
    }
}

void asteroids::Asteroids::createExplosion(pg::fVec2& position)
{
    auto entity = pg::game::makeEntity<pg::Transform2D>(game.getRegistry(), pg::Transform2D{.pos{position}});

    auto animation = std::make_shared<pg::FramedSprite>(pg::SpriteFactory::makeFramedSprite(
        game.getApp().getRenderer(),
        8,
        4,
        "../data/effects/explosion_1_8x4.png",
        [this, entity, max_frames = 32](auto& frame_number) {
            ++frame_number;
            if (frame_number >= max_frames)
            {
                //
                game.getDispatcher().enqueue<pg::game::events::DestroyEntityEvent>({entity});
            }
        }));
    pg::game::addComponents<pg::game::Drawable>(game.getRegistry(), entity, pg::game::Drawable{animation});

    // pg::game::Drawable,
    // pg::game::Drawable{animation},
}
