#pragma once
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/entities/Drawable.hpp>

#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/math/VecUtils.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/UpdateSystem.hpp>

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
        auto& scene = game->createScene("start");
        auto& systems = scene.getSystems();
        systems.emplace_back(std::make_unique<galaxy::RenderSystem>(*game));
        systems.emplace_back(std::make_unique<galaxy::UpdateSystem>(*game));

        game->getKeyStateMap().registerMouseRelativeDraggedCallback([&scene](auto pos, auto state) {
            scene.getGlobalTransform().pos += vec_cast<float>(pos) * (1.0f / scene.getGlobalTransform().scale);
        });

        game->getKeyStateMap().registerMouseWheelCallback([&scene](auto pos) {
            scene.getGlobalTransform().scale *= std::powf(1.1f, pos[1]);
            scene.getGlobalTransform().scale[0] = std::clamp(scene.getGlobalTransform().scale[0], 0.5f, 10.0f);
            scene.getGlobalTransform().scale[1] = std::clamp(scene.getGlobalTransform().scale[1], 0.5f, 10.0f);
        });

        game->getKeyStateMap().registerKeyCallback(SDLK_SPACE, [&scene](auto) { scene.getGlobalTransform() = {}; });
        setupStarSystems();
        // setupRegularGrid();
        game->switchScene("start");
    }

    void run() { game->loop(); }

private:
    void setupStarSystems()
    {
        auto dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");

        for (auto i : std::ranges::iota_view{1, 5000})
        {
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::StarSystemState>(
                game->getRegistry(),
                {.pos{pg::getPointInCircle(375)}, .scale{0.025, 0.025}},
                pg::game::Drawable{dot_sprite},
                galaxy::StarSystemState{});
        }
    }

    void setupRegularGrid()
    {
        // centered around 0,0
        auto windowRect = game->getSingleton<pg::game::WindowDetails>().windowRect;
        auto dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");
        auto midpoint = pg::dimsFromRect<float>(windowRect) * 0.5f;

        for (auto x : std::ranges::iota_view{0, windowRect.w / 10})
        {
            for (auto y : std::ranges::iota_view{0, windowRect.h / 10})
            {
                pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::StarSystemState>(
                    game->getRegistry(),
                    {.pos{midpoint - 10.0f * pg::fVec2{static_cast<float>(x), static_cast<float>(y)}},
                     .scale{0.025, 0.025}},
                    pg::game::Drawable{dot_sprite},
                    galaxy::StarSystemState{(x == windowRect.w / 20 && y == windowRect.h / 20)
                                                ? ColonizationStatus::Colonized
                                                : ColonizationStatus::Unexplored});
            }
        }
    }

private:
    std::unique_ptr<pg::game::Game> game;
};

} // namespace galaxy