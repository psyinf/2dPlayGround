#include "Asteroids.h"

#include "core/RegistryHelper.h"
#include "entities/Entities.h"
#include <Factories.h>
#include <SDLBackgoundSprite.h>
#include <core/Game.h>
#include <random>
#include <systems/Lasers.h>

struct AsteroidsRandomGen
{
    static inline std::random_device                     rd{};
    static inline std::normal_distribution<float> speed{0.2, 5};
    static inline std::uniform_int_distribution   pos{-200, 1024};

    static auto makeInitial()
    {
        auto velocity = pg::fVec2{0, std::fabs(speed(rd))};
        auto position = pg::fVec2{static_cast<float>(pos(rd)), 0};
        return std::make_pair(position, velocity);
    }
};

std::optional<std::pair<entt::entity, entt::entity>> getAsteroidWeaponPair(game::Game&                    game,
                                                                           const game::events::Collision& collision)
{
    auto                                  isAsteroidE1 = game.getRegistry().all_of<game::Asteroids::tag>(collision.c1);
    auto                                  isAsteroidE2 = game.getRegistry().all_of<game::Asteroids::tag>(collision.c2);
    auto                                  isLaserE1 = game.getRegistry().all_of<game::Lasers::tag>(collision.c1);
    auto                                  isLaserE2 = game.getRegistry().all_of<game::Lasers::tag>(collision.c2);
    std::pair<entt::entity, entt::entity> retVal;

    if (!isAsteroidE1 && !isAsteroidE2) { return std::nullopt; }
    if (!isLaserE1 && !isLaserE2) { return std::nullopt; }
    if (isAsteroidE1) { retVal.first = collision.c1; }
    else { retVal.first = collision.c2; }
    if (isLaserE1) { retVal.second = collision.c1; }
    else { retVal.second = collision.c2; }
    return retVal;
}

void game::Asteroids::setup()
{
    

    for (int i = 0; i < 5; ++i)
    {
        auto posVel = AsteroidsRandomGen::makeInitial();
        createAsteroid("../data/meteorBrown_big1.png", posVel.first, posVel.second);
    }
    game.getDispatcher().sink<game::events::Collision>().connect<&Asteroids::handleEvent>(this);
}

void game::Asteroids::createAsteroid(const std::string_view resource, const pg::fVec2& position, const pg::fVec2& velocity)
{
    auto sprite = game.getResourceCache().load<pg::Sprite>(
        std::string(resource), [this](const auto& e) {
        return pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), e);
    });
    game::makeEntity<Drawable, pg::Transform, Dynamics, pg::BoundingSphere, game::PassiveCollider, tag>
        
        (game.getRegistry(),            //
         Drawable{sprite},   //
         pg::Transform{.pos{position}}, //
         {.velocity{velocity}},
         {pg::BoundingSphere::fromRectangle(sprite->getDimensions())},
         {},
         {} //
        );
}

void game::Asteroids::handle(const FrameStamp& frameStamp)
{
    std::random_device              rd;
    std::normal_distribution<float> dist(1, 2);
    std::normal_distribution<float> speed(0.2, 3);
    std::uniform_int_distribution   pos(-200, 1024);
    // TODO: move to generic dynamics-system
    auto view = game.getRegistry().view<tag, pg::Transform, game::Dynamics>();
    for (auto& entity : view)
    {
        auto&& [transform, dynamics] = view.get<pg::Transform, game::Dynamics>(entity);
        transform.pos[1] += std::ceil(dynamics.velocity[1]);

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
        game.getRegistry().destroy(collisionPair->first);
        game.getRegistry().destroy(collisionPair->second);
        // trigger explosion
        // game.getDispatcher().trigger<events::Collision>( {entity_a, entity_b});
    }
}
