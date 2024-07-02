#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <memory>

#include <pgEngine/core/LoadSave.hpp>
#include <serializer/ConfigSerializer.hpp>

namespace galaxy {

class GalaxyScene : public pg::game::Scene
{
public:
    GalaxyScene(pg::game::Game& game)
      : pg::game::Scene(game)
    {
        galaxy::config::Galaxy galaxy_config;
        pg::save("../data/galaxy_default_config.json", galaxy_config);

        galaxyConfig = pg::load<galaxy::config::Galaxy>("../data/galaxy_config.json", galaxy_config);
    }

    void start() override
    {
        auto& systems = getSystems();
        auto& game = getGame();
        systems.emplace_back(std::make_unique<galaxy::RenderSystem>(game));

        systems.emplace_back(std::make_unique<galaxy::UpdateSystem>(game));
        systems.emplace_back(std::make_unique<galaxy::PickingSystem>(game));
        systems.emplace_back(std::make_unique<galaxy::DroneSystem>(game));
        systems.emplace_back(std::make_unique<galaxy::LifetimeSystem>(game));
        systems.emplace_back(std::make_unique<galaxy::BehaviorSystem>(game));
        systems.emplace_back(std::make_unique<galaxy::StatsSystem>(game));

        setupKeyHandler();
        setupOverlay();
        setupSelectionMarker();
        setupGalaxy();

        addSingleton_as<const pg::Color&>("galaxy.star.default_color", galaxyConfig.star.default_color);
        addSingleton_as<const galaxy::config::Galaxy&>("galaxy.config", galaxyConfig);

        Scene::start();
    }

    void stop() override { Scene::stop(); }

private:
    void setupKeyHandler()
    {
        auto& game = getGame();
        addSingleton_as<pg::KeyStateMap&>("galaxy.keyStateMap", game.getKeyStateMap());

        game.getKeyStateMap().registerMouseRelativeDraggedCallback([this](auto pos, auto state) {
            if (state & SDL_BUTTON_LMASK)
            {
                const auto absolute_drag_distance = pg::vec_cast<float>(pos);
                if (pg::lengthSquared(absolute_drag_distance) < 4.0f) { return; };
                getGlobalTransform().pos += absolute_drag_distance * (1.0f / getGlobalTransform().scale);
                isDragging = true;
            }
        });

        game.getKeyStateMap().registerMousePressedCallback([this](auto pos, auto button, bool pressed) {
            if (isDragging && !pressed)
            {
                isDragging = false;
                return;
            }
            if (button == SDL_BUTTON_LEFT && !pressed)
            {
                // fire a pick event. TODO: distinguish between drag and click
                auto world_pos = pg::vec_cast<float>(pos);
                auto windowRect = getSingleton<pg::game::WindowDetails>().windowRect;
                auto globalTransform = getGlobalTransform();
                world_pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
                world_pos = (world_pos) * (1.0f / globalTransform.scale);
                world_pos -= globalTransform.pos;
                auto event = galaxy::events::PickEvent{
                    .screen_position{pos}, .world_position{world_pos}, .scale{globalTransform.scale}};
                getGame().getDispatcher().trigger(event);
            };
        });

        game.getKeyStateMap().registerMouseWheelCallback([this](auto pos) {
            auto zoom = galaxyConfig.zoom;
            getGlobalTransform().scale *= static_cast<float>(std::pow(zoom.factor + 1.0f, pos[1]));
            getGlobalTransform().scale[0] = std::clamp(getGlobalTransform().scale[0], zoom.min, zoom.max);
            getGlobalTransform().scale[1] = std::clamp(getGlobalTransform().scale[1], zoom.min, zoom.max);
        });

        game.getKeyStateMap().registerKeyCallback(SDLK_SPACE, [this](auto) { getGlobalTransform() = {}; });
        //         game.getKeyStateMap().registerKeyCallback(
        //             SDLK_d, [this](auto) { drawDebugItems = !drawDebugItems; }, true);
        // TODO: in Game class
        game.getApp().getEventHandler().windowEvent = [this](const SDL_WindowEvent e) {
            if (e.event == SDL_WINDOWEVENT_RESIZED)
            {
                auto& windowDetails = getSingleton<pg::game::WindowDetails>();
                windowDetails.windowRect.w = e.data1;
                windowDetails.windowRect.h = e.data2;
            }
        };

        // TODO: ESC/ESC
        game.getKeyStateMap().registerKeyCallback(SDLK_ESCAPE, [this](auto) { getGlobalTransform() = {}; });
        //
    }

    void setupOverlay()
    {
        pg::game::makeEntity<pg::game::GuiDrawable>(getGame().getRegistry(),
                                                    {std::make_unique<pg::game::GuiBegin>(), pg::game::DRAWABLE_FIRST});

        pg::game::makeEntity<pg::game::GuiDrawable>(getRegistry(),
                                                    {std::make_unique<pg::game::GuiEnd>(), pg::game::DRAWABLE_LAST});

        pg::game::makeEntity<pg::game::GuiDrawable>(
            getRegistry(),
            {std::make_unique<galaxy::gui::DashBoardWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});

        pg::game::makeEntity<pg::game::GuiDrawable>(getRegistry(),
                                                    {std::make_unique<galaxy::gui::SystemInfoWidget>(getGame())});

        pg::game::makeEntity<pg::game::GuiDrawable>(getRegistry(),
                                                    {std::make_unique<galaxy::gui::StatsWidget>(getGame())});
    }

    void setupGalaxy()
    {
        galaxyQuadtree = std::make_unique<pg::Quadtree<entt::entity>>(pg::fBox{{-750, -750}, {1500, 1500}});
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::normal_distribution<float> d(0.0f, 150.0f);
        std::normal_distribution<float> star_size_dist(0.0075f, 0.0025f);

        auto dot_sprite = getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");

        for ([[maybe_unused]] auto i : std::ranges::iota_view{0, 10000})
        {
            auto new_pos = pg::fVec2{d(gen), d(gen)};
            auto new_size = star_size_dist(gen) * pg::fVec2{1.0f, 1.0f};
            auto entity = pg::game::makeEntity<pg::Transform2D,
                                               pg::game::Drawable,
                                               galaxy::StarSystemState,
                                               galaxy::Faction,
                                               pg::game::RenderState> //
                (getRegistry(),
                 {.pos{new_pos}, .scale{new_size}, .scaleSpace{pg::TransformScaleSpace::Local}},
                 pg::game::Drawable{dot_sprite},
                 galaxy::StarSystemState{},
                 galaxy::Faction{"None"},
                 {});

            galaxyQuadtree->insert({new_pos, new_size}, entity, galaxyQuadtree->root);
        }
        // add some background
        auto background_sprite = getTypedResourceCache<pg::Sprite>().load("../data/background/milky_way_blurred.png");
        auto states = pg::States{};
        states.push(pg::TextureAlphaState{static_cast<uint8_t>(galaxyConfig.background.opacity * 255)});
        pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::game::RenderState>(
            getRegistry(), {.pos{0, 0}, .scale{0.5, 0.5}}, pg::game::Drawable{background_sprite}, {std::move(states)});

        addSingleton_as<const pg::Quadtree<entt::entity>&>("galaxy.quadtree", *galaxyQuadtree);
    }

    void setupQuadtreeDebug()
    {
        CollectBoundsVisitor<pg::QuadTreeNode<entt::entity>> visitor;
        galaxyQuadtree->root->accept(visitor);
        auto windowRect = getSingleton<pg::game::WindowDetails>().windowRect;
        for (const auto& box : visitor.results)
        {
            auto box_prim = std::make_shared<pg::BoxPrimitive>(box, pg::Color{100, 0, 0, 50});

            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::tags::MarkerTag>(
                getRegistry(), {.pos{}, .scale{1, 1}}, pg::game::Drawable{box_prim}, {});
        }
    }

    void setupSelectionMarker()
    {
        auto         dot_texture = getTypedResourceCache<sdl::Texture>().load("../data/reticle.png");
        auto         marker = std::make_shared<pg::Sprite>(dot_texture);
        entt::entity markers =
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::game::RenderState, pg::tags::MarkerTag>(
                getRegistry(), {.pos{0, 0}, .scale{0.015f, 0.015f}}, pg::game::Drawable{marker}, {}, {});
        addSingleton_as<entt::entity>("galaxy.debug.marker", markers);
    }

private:
    std::unique_ptr<pg::Quadtree<entt::entity>> galaxyQuadtree;
    config::Galaxy                              galaxyConfig;
    bool                                        isDragging{};
};

} // namespace galaxy