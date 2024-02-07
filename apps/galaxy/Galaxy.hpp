#pragma once
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/entities/Drawable.hpp>

#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/math/VecUtils.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/UpdateSystem.hpp>
#include <systems/PickingSystem.hpp>

#include "entities/StarSystem.hpp"
#include "events/PickEvent.hpp"
#include <pgEngine/math/Random.hpp>
#include <pgGame/entities/WindowDetails.hpp>

#include <cmath>

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
        systems.emplace_back(std::make_unique<galaxy::PickingSystem>(*game));
        // TODO: maybe encapsulate this into a class
        // TODO: add a mechanism that wathces the mouse position and triggers a pick event in case it was not moved for
        // a while

        game->getKeyStateMap().registerMouseRelativeDraggedCallback([&scene, this](auto pos, auto state) {
            if (state & SDL_BUTTON_LMASK)
            {
                const auto absolute_drag_distance = vec_cast<float>(pos);
                if (lengthSquared(absolute_drag_distance) < 4.0f) { return; };
                scene.getGlobalTransform().pos += absolute_drag_distance * (1.0f / scene.getGlobalTransform().scale);
                isDragging = true;
            }
        });

        game->getKeyStateMap().registerMousePressedCallback([this](auto pos, auto button, bool pressed) {
            if (isDragging && !pressed)
            {
                isDragging = false;
                return;
            }
            if (button == SDL_BUTTON_LEFT && !pressed)
            {
                // fire a pick event. TODO: distinguish between drag and click
                auto event = galaxy::events::PickEvent{.screen_position{pos}};
                game->getDispatcher().trigger(event);
            };
        });

        game->getKeyStateMap().registerMouseWheelCallback([&scene](auto pos) {
            scene.getGlobalTransform().scale *= static_cast<float>(std::pow(1.1f, pos[1]));
            scene.getGlobalTransform().scale[0] = std::clamp(scene.getGlobalTransform().scale[0], 0.5f, 10.0f);
            scene.getGlobalTransform().scale[1] = std::clamp(scene.getGlobalTransform().scale[1], 0.5f, 10.0f);
        });

        game->getKeyStateMap().registerKeyCallback(SDLK_SPACE, [&scene](auto) { scene.getGlobalTransform() = {}; });
        // setupStarSystems();
        //  setupRegularGrid();
        setupGalaxy();
        game->switchScene("start");
    }

    void run() { game->loop(); }

private:
    void setupGalaxy()
    {
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::normal_distribution<float> d(0, 150);
        std::normal_distribution<float> star_size_dist(0.0075, 0.0025);

        auto dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");

        for (auto i : std::ranges::iota_view{1, 4000})
        {
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::StarSystemState>(
                game->getRegistry(),
                {.pos{d(gen), d(gen)}, .scale{star_size_dist(gen) * pg::fVec2{1.0f, 1.0f}}},
                pg::game::Drawable{dot_sprite},
                galaxy::StarSystemState{});
        }
        // add some background
        auto background_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/background/milky_way.jpg");
        pg::game::makeEntity<pg::Transform2D, pg::game::Drawable>(
            game->getRegistry(), {.pos{0, 0}, .scale{0.5, 0.5}}, pg::game::Drawable{background_sprite});
    }

    void setupStarSystems()
    {
        auto dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");

        for (auto i : std::ranges::iota_view{1, 5000})
        {
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::StarSystemState>(
                game->getRegistry(),
                {.pos{pg::getPointInCircle(375)}, .scale{0.0125, 0.0125}},
                pg::game::Drawable{dot_sprite},
                galaxy::StarSystemState{});
        }
        // add some background
        auto background_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/background/milky_way.jpg");
        pg::game::makeEntity<pg::Transform2D, pg::game::Drawable>(
            game->getRegistry(), {.pos{0, 0}, .scale{0.5, 0.5}}, pg::game::Drawable{background_sprite});
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
    bool                            isDragging;
};

} // namespace galaxy