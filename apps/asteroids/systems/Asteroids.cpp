#include "Asteroids.h"

#include <core/Game.h>
#include "core/RegistryHelper.h"
#include <Factories.h>
#include <SDLBackgoundSprite.h>
#include "entities/Entities.h"
#include <random>


void game::Asteroids::setup()
{
    

    static std::random_device              rd;
    std::normal_distribution<float> speed(0.2, 5);
    std::uniform_int_distribution pos(-200, 1024);
    for (int i = 0; i < 5; ++i)
    {
        //TODO: make textures shareable
        auto     sprite = pg::SpriteFactory::makeSprite(game.getApp().getRenderer(), "../data/meteorBrown_big1.png");
        Drawable d(std::make_unique<pg::Sprite>(std::move(sprite)));
        game::makeEntity<Drawable, pg::Transform, Dynamics, tag>

            (game.getRegistry(),                                  //
             std::move(d),                                        //
             pg::Transform{.pos{static_cast<float>(pos(rd)), 0}}, //
             {.velocity{0, std::fabs(speed(rd))}},
             {} //
            );
    }
}

void game::Asteroids::handle(const FrameStamp& frameStamp)
{
    std::random_device              rd;
    std::normal_distribution<float> dist(1, 2);
    std::normal_distribution<float> speed(0.2, 5);
    std::uniform_int_distribution   pos(-200, 1024);
    auto  view = game.getRegistry().view<tag, pg::Transform, game::Dynamics>();
    for (auto& entity : view) {
        auto&& [transform, dynamics] = view.get<pg::Transform, game::Dynamics>(entity);
        transform.pos[1] += std::ceil(dynamics.velocity[1]);

        if (transform.pos[1] >= 1000)
        {
            transform.pos = {static_cast<float>(pos(rd)), -100}; //
            dynamics.velocity[1] = std::fabs(speed(rd));
        }
    }
    
}
