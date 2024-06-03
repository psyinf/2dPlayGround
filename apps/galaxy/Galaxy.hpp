#pragma once
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/components/Drawable.hpp>

#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/math/VecUtils.hpp>
#include <pgEngine/math/Quadtree.hpp>

#include <components/Tags.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/UpdateSystem.hpp>
#include <systems/PickingSystem.hpp>
#include <systems/DroneSystem.hpp>
#include <systems/LifetimeSystem.hpp>
#include <systems/BehaviorSystem.hpp>

#include "components/StarSystem.hpp"
#include "components/Faction.hpp"
#include "pgGame/components/RenderState.hpp"
#include "events/PickEvent.hpp"
#include <pgEngine/math/Random.hpp>
#include <pgGame/components/WindowDetails.hpp>
#include <Config.hpp>
#include <cmath>
#include <ranges>

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
        systems.emplace_back(std::make_unique<galaxy::DroneSystem>(*game));
        systems.emplace_back(std::make_unique<galaxy::LifetimeSystem>(*game));
        systems.emplace_back(std::make_unique<galaxy::BehaviorSystem>(*game));
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
                auto event = galaxy::events::PickEvent{
                    .screen_position{pos}, .world_position{world_pos}, .scale{globalTransform.scale}};
                game->getDispatcher().trigger(event);
            };
        });

        game->getKeyStateMap().registerMouseWheelCallback([&scene, this](auto pos) {
            auto zoom = galaxyConfig.zoom;
            scene.getGlobalTransform().scale *= static_cast<float>(std::pow(zoom.factor + 1.0f, pos[1]));
            scene.getGlobalTransform().scale[0] = std::clamp(scene.getGlobalTransform().scale[0], zoom.min, zoom.max);
            scene.getGlobalTransform().scale[1] = std::clamp(scene.getGlobalTransform().scale[1], zoom.min, zoom.max);
        });

        game->getKeyStateMap().registerKeyCallback(SDLK_SPACE, [&scene](auto) { scene.getGlobalTransform() = {}; });
        game->getKeyStateMap().registerKeyCallback(
            SDLK_d, [this](auto) { drawDebugItems = !drawDebugItems; }, true);
        // setupStarSystems();
        // setupRegularGrid();
        setupGalaxy();
        setupQuadtreeDebug();
        setupSelectionMarker();
        setupConfig();

        game->switchScene("start");
        // TODO: add some wrapper that holds a map of registered singleton names used as configuration items
        // Better: generic mechanism that wraps a config struct
        game->addSingleton_as<const bool&>("galaxy.debug.draw", drawDebugItems);
        game->addSingleton_as<const bool&>("galaxy.draw_quadtree", drawQuadTree);
        game->addSingleton_as<const pg::Quadtree<entt::entity>&>("galaxy.quadtree", *galaxyQuadtree);
    }

    void run() { game->loop(); }

private:
    void setupConfig()
    {
        game->addSingleton_as<const pg::Color&>("galaxy.star.default_color", galaxyConfig.star.default_color);
        game->addSingleton_as<const galaxy::config::Galaxy&>("galaxy.config", galaxyConfig);
    }

    void setupSelectionMarker()
    {
        auto         dot_texture = game->getTypedResourceCache<sdl::Texture>().load("../data/reticle.png");
        auto         marker = std::make_shared<pg::Sprite>(dot_texture);
        entt::entity markers =
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::tags::DebugRenderingItemTag>(
                game->getRegistry(), {.pos{0, 0}, .scale{0.015, 0.015}}, pg::game::Drawable{marker}, {});
        game->addSingleton_as<entt::entity>("galaxy.debug.marker", markers);
    }

    void setupQuadtreeDebug()
    {
        if (!drawQuadTree) { return; }
        CollectBoundsVisitor<pg::QuadTreeNode<entt::entity>> visitor;
        galaxyQuadtree->root->accept(visitor);
        auto windowRect = game->getSingleton<pg::game::WindowDetails>().windowRect;
        for (const auto& box : visitor.results)
        {
            // continue;
            auto box_prim = std::make_shared<pg::BoxPrimitive>(box, pg::Color{100, 0, 0, 50});

            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::tags::DebugRenderingItemTag>(
                game->getRegistry(), {.pos{}, .scale{1, 1}}, pg::game::Drawable{box_prim}, {});
        }
    }

    void setupGalaxy()
    {
        galaxyQuadtree = std::make_unique<pg::Quadtree<entt::entity>>(pg::fBox{{-750, -750}, {1500, 1500}});
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::normal_distribution<float> d(0, 150);
        std::normal_distribution<float> star_size_dist(0.0075, 0.0025);

        auto dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");

        for (auto i : std::ranges::iota_view{1, 10000})
        {
            auto new_pos = pg::fVec2{d(gen), d(gen)};
            auto new_size = star_size_dist(gen) * pg::fVec2{1.0f, 1.0f};
            auto entity = pg::game::makeEntity<pg::Transform2D,
                                               pg::game::Drawable,
                                               galaxy::StarSystemState,
                                               galaxy::Faction,
                                               pg::game::RenderState> //
                (game->getRegistry(),
                 {.pos{new_pos}, .scale{new_size}, .scaleSpace{pg::TransformScaleSpace::Local}},
                 pg::game::Drawable{dot_sprite},
                 galaxy::StarSystemState{},
                 galaxy::Faction{"None"},
                 {});

            galaxyQuadtree->insert({new_pos, new_size}, entity, galaxyQuadtree->root);
        }
        // add some background
        auto background_sprite =
            game->getTypedResourceCache<pg::Sprite>().load("../data/background/milky_way_blurred.png");
        auto states = pg::States{};
        states.push(pg::TextureAlphaState{static_cast<uint8_t>(galaxyConfig.background.opacity * 255)});
        pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::game::RenderState>(
            game->getRegistry(),
            {.pos{0, 0}, .scale{0.5, 0.5}},
            pg::game::Drawable{background_sprite},
            {std::move(states)});
    }

private:
    std::unique_ptr<pg::game::Game>             game;
    std::unique_ptr<pg::Quadtree<entt::entity>> galaxyQuadtree;
    bool                                        isDragging{};
    bool                                        drawDebugItems{true};
    bool                                        drawQuadTree{false};
    config::Galaxy                              galaxyConfig;
};

} // namespace galaxy