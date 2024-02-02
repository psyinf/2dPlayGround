#pragma once
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <pgEngine/primitives/Sprite.hpp>
#include <systems/RenderSystem.hpp>
#include "entities/StarSystem.hpp"
#include <pgEngine/math/Random.hpp>
#include <pgGame/entities/WindowDetails.hpp>

namespace galaxy {
class GalacticCore
{
public:
    GalacticCore()
      : game(std::make_unique<pg::game::Game>())
    {
    }

    void setup()
    {
        auto& systems = game->getSystems();
        systems.emplace_back(std::make_unique<galaxy::RenderSystem>(*game));

        game->setup();
        setupStarSystems();
    }

    void run() { game->loop(); }

private:
    void setupStarSystems()
    {
        auto dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");
        //         for (auto i = 0; i < 100; ++i)
        //         {
        //             for (auto j = 0; j < 100; ++j)
        //             {
        //                 pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::StarSystemState>(
        //                     game->getRegistry(),
        //                     {.pos{100 + static_cast<float>(i) * 100, 100 + static_cast<float>(j) * 100}, .scale{0.05,
        //                     0.05}}, pg::game::Drawable{dot_sprite}, galaxy::StarSystemState{});
        //             }
        //         }
        auto window_details = game->getSingleton<pg::game::WindowDetails>();
        auto midpoint = pg::fVec2{window_details.windowRect.w / 2.0f, window_details.windowRect.h / 2.0f};

        for (auto i : std::ranges::iota_view{1, 5000})
        {
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::StarSystemState>(
                game->getRegistry(),
                {.pos{pg::getPointInCircle(375) + midpoint}, .scale{0.025, 0.025}},
                pg::game::Drawable{dot_sprite},
                galaxy::StarSystemState{});
        }
    }

private:
    std::unique_ptr<pg::game::Game> game;
};

} // namespace galaxy