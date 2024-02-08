#pragma once
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/entities/Drawable.hpp>

#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/math/VecUtils.hpp>
#include <pgEngine/math/Quadtree.hpp>

#include <entities/Tags.hpp>
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
        // TODO: add a mechanism that watches the mouse position and triggers a pick event in case it was not moved for
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

        game->getKeyStateMap().registerMousePressedCallback([this, &scene](auto pos, auto button, bool pressed) {
            if (isDragging && !pressed)
            {
                isDragging = false;
                return;
            }
            if (button == SDL_BUTTON_LEFT && !pressed)
            {
                // fire a pick event. TODO: distinguish between drag and click
                auto world_pos = vec_cast<float>(pos);
                auto windowRect = game->getSingleton<pg::game::WindowDetails>().windowRect;
                auto globalTransform = scene.getGlobalTransform();
                world_pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
                world_pos = (world_pos) * (1.0f / globalTransform.scale);
                world_pos -= globalTransform.pos;
                auto event = galaxy::events::PickEvent{.screen_position{pos}, .world_position{world_pos}};
                game->getDispatcher().trigger(event);
            };
        });

        game->getKeyStateMap().registerMouseWheelCallback([&scene](auto pos) {
            scene.getGlobalTransform().scale *= static_cast<float>(std::pow(1.1f, pos[1]));
            scene.getGlobalTransform().scale[0] = std::clamp(scene.getGlobalTransform().scale[0], 0.5f, 10.0f);
            scene.getGlobalTransform().scale[1] = std::clamp(scene.getGlobalTransform().scale[1], 0.5f, 10.0f);
        });

        game->getKeyStateMap().registerKeyCallback(SDLK_SPACE, [&scene](auto) { scene.getGlobalTransform() = {}; });
        game->getKeyStateMap().registerKeyCallback(
            SDLK_d, [this](auto) { drawDebugItems = !drawDebugItems; }, true);
        // setupStarSystems();
        //  setupRegularGrid();
        setupGalaxy();
        setupQuadtreeDebug();
        game->switchScene("start");
        // TODO: add some wrapper that holds a map of registered singleton names used as configuration items
        // Better: generic mechanism that wraps a config struct
        game->addSingleton_as<const bool&>("galaxy.debug.draw", drawDebugItems);
        game->addSingleton_as<const pg::Quadtree&>("galaxy.quadtree", *galaxyQuadtree);
    }

    void run() { game->loop(); }

private:
    void setupQuadtreeDebug()
    {
        CollectBoundsVisitor<pg::QuadTreeNode> visitor;
        galaxyQuadtree->root->accept(visitor);
        auto windowRect = game->getSingleton<pg::game::WindowDetails>().windowRect;
        for (const auto& box : visitor.results)
        {
            auto box_prim = std::make_shared<pg::BoxPrimitive>(box);

            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::tags::DebugRenderingItemTag>(
                game->getRegistry(), {.pos{}, .scale{1, 1}}, pg::game::Drawable{box_prim}, {});
        }
        auto         dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");
        entt::entity marker =
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::tags::DebugRenderingItemTag>(
                game->getRegistry(), {.pos{}, .scale{0.05, 0.05}}, pg::game::Drawable{dot_sprite}, {});
        game->addSingleton_as<entt::entity>("galaxy.debug.marker", marker);
    }

    void setupGalaxy()
    {
        galaxyQuadtree = std::make_unique<pg::Quadtree>(pg::fBox{{-750, -750}, {1500, 1500}});
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::normal_distribution<float> d(0, 150);
        std::normal_distribution<float> star_size_dist(0.0075, 0.0025);

        auto dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");

        for (auto i : std::ranges::iota_view{1, 4000})
        {
            auto new_pos = pg::fVec2{d(gen), d(gen)};
            auto new_size = star_size_dist(gen) * pg::fVec2{1.0f, 1.0f};
            galaxyQuadtree->insert({new_pos, new_size}, galaxyQuadtree->root);
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::StarSystemState>(
                game->getRegistry(),
                {.pos{new_pos}, .scale{new_size}},
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
    std::unique_ptr<pg::Quadtree>   galaxyQuadtree;
    bool                            isDragging;
    bool                            drawDebugItems;
};

} // namespace galaxy