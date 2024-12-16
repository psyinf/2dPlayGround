#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <memory>

#include <pgEngine/core/LoadSave.hpp>
#include <pgEngine/resources/SpriteResource.hpp>

#include <gui/StatsWidget.hpp>
#include <gui/SystemInfo.hpp>
#include <gui/DashBoardWidget.hpp>
#include <gui/LoadResourcesWidget.hpp>
#include <gui/MainBarWidget.hpp>

#include <serializer/ConfigSerializer.hpp>

#include <systems/GuiRenderSystem.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/VecUtils.hpp>

#include <pgOrbit/StarParameters.hpp>
#include <systems/UpdateStarsSystem.hpp>
#include <systems/PickingSystem.hpp>
#include <systems/DroneSystem.hpp>
#include <systems/LifetimeSystem.hpp>
#include <systems/BehaviorSystem.hpp>
#include <systems/StatsSystem.hpp>
#include <components/Tags.hpp>

#include <pgGame/core/RegistryHelper.hpp>
#include <components/Tags.hpp>
#include <entt/entt.hpp>
#include <components/singletons/RegisteredPreloaders.hpp>
#include <pgEngine/resources/SpriteResource.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class GalaxyScene : public pg::game::Scene
{
public:
    GalaxyScene(pg::game::Game& game, pg::game::SceneConfig&& scene_cfg)
      : pg::game::Scene(game, std::move(scene_cfg))
    {
        galaxy::config::Galaxy galaxy_config;
        pg::save("../data/galaxy_default_config.json", galaxy_config);

        galaxyConfig = pg::load<galaxy::config::Galaxy>("../data/galaxy_config.json", galaxy_config);
        // add preloaders
        auto&                    preLoaders = game.getSingleton<singleton::RegisteredLoaders>();
        std::vector<std::string> files = {
            "../data/reticle.png", "../data/background/milky_way_blurred.png", "../data/circle_05.png"};

        for (auto& file : files)
        {
            if (preLoaders.loaders.contains(file)) { continue; }
            auto& game = getGame();
            auto  loader = [&game, file](PercentCompleted& percentLoaded) {
                percentLoaded[file] = 0.0f;
                game.getResourceManager().get().load<pg::Sprite, sdl::Renderer&>(file, game.getApp().getRenderer());
                percentLoaded[file] = 1.0f;
            };
            preLoaders.loaders.emplace(file, loader);
        }

        /// add markov chain for names
        auto loader = [this](PercentCompleted& completion) {
            std::ifstream fileStreamIn("../data/text/corpi/stars.txt", std::ios_base::binary);
            std::ifstream fileStreamIn2("../data/text/corpi/boys.txt", std::ios_base::binary);
            auto          file1_size = std::filesystem::file_size("../data/text/corpi/stars.txt");
            auto          file2_size = std::filesystem::file_size("../data/text/corpi/boys.txt");
            auto          total_size = file1_size + file2_size;
            auto          resource = "markov";
            pg::generators::MarkovFrequencyMap<4> fmg;

            while (!fileStreamIn.eof() && fileStreamIn)
            {
                std::string word;
                fileStreamIn >> word;
                // current file position
                auto pos = fileStreamIn.tellg();
                completion[resource] = static_cast<float>(pos) / total_size;
                fmg.add(word);
            }

            while (!fileStreamIn2.eof() && fileStreamIn2)
            {
                std::string word;
                fileStreamIn2 >> word;
                auto pos = fileStreamIn2.tellg();
                completion[resource] = static_cast<float>(pos) / total_size;
                fmg.add(word);
            }
            if (fmg.size() == 0)
            {
                fmg.add("empty");
                // log
                spdlog::error("No words loaded from file");
            }
            getGame().addSingleton_as<pg::generators::MarkovFrequencyMap<4>>("markovFrequencyMap", fmg);
        };

        preLoaders.loaders.emplace("markov", std::move(loader));
    }

    virtual ~GalaxyScene() = default;

    void start() override
    {
        if (!started())
        {
            setupKeyHandler();
            setupOverlay();
            setupSelectionMarker();
            setupGalaxy();

            addSingleton_as<const pg::Color&>("galaxy.star.default_color", galaxyConfig.star.default_color);
            addSingleton_as<const galaxy::config::Galaxy&>("galaxy.config", galaxyConfig);

            Scene::start();
        }
    }

    void stop() override { Scene::stop(); }

private:
    void setupKeyHandler()
    {
        // TODO: keystate map per scene
        auto& game = getGame();
        addSingleton_as<pg::KeyStateMap&>("galaxy.keyStateMap", getKeyStateMap());

        getKeyStateMap().registerMouseRelativeDraggedCallback([this](auto pos, auto state) {
            if (state & SDL_BUTTON_LMASK)
            {
                const auto absolute_drag_distance = pg::vec_cast<float>(pos);
                if (pg::lengthSquared(absolute_drag_distance) < 4.0f) { return; };
                getGlobalTransform().pos += absolute_drag_distance * (1.0f / getGlobalTransform().scale);
                isDragging = true;
            }
        });

        getKeyStateMap().registerMousePressedCallback([this](auto pos, auto button, bool pressed) {
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

        getKeyStateMap().registerMouseDoubleClickedCallback(
            [this](auto [[maybe_unused]] /*pos*/, auto button, bool pressed) {
                if (!pressed && button == SDL_BUTTON_LEFT)
                {
                    auto selected_entity =
                        getGame().getSingleton_or<PickedEntity>("picked.entity", PickedEntity{}).entity;
                    if (selected_entity == entt::null) { return; }
                    // TODO: entering the system should be possible for systems explored/occupied by current faction

                    getGame().getGlobalDispatcher().enqueue<pg::game::events::SwitchSceneEvent>({"system"});
                }
            });

        getKeyStateMap().registerMouseWheelCallback([this](auto pos) {
            auto zoom = galaxyConfig.zoom;
            getGlobalTransform().scale *= static_cast<float>(std::pow(zoom.factor + 1.0f, pos[1]));
            getGlobalTransform().scale[0] = std::clamp(getGlobalTransform().scale[0], zoom.min, zoom.max);
            getGlobalTransform().scale[1] = std::clamp(getGlobalTransform().scale[1], zoom.min, zoom.max);
        });

        getKeyStateMap().registerKeyCallback(SDLK_SPACE, [this](auto) { getGlobalTransform() = {}; });
        //         getKeyStateMap().registerKeyCallback(
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
        getKeyStateMap().registerKeyCallback(SDLK_ESCAPE, [this](auto) { getGlobalTransform() = {}; });
        //
    }

    void setupOverlay()
    {
        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiBegin>(), pg::game::DRAWABLE_FIRST});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiEnd>(), pg::game::DRAWABLE_LAST});

        pg::game::makeEntity<pg::game::GuiDrawable>(
            getSceneRegistry(),
            {std::make_unique<galaxy::gui::DashBoardWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<galaxy::gui::SystemInfoWidget>(getGame())});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<galaxy::gui::StatsWidget>(getGame())});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<galaxy::gui::MainBarWidget>(getGame())});
    }

    void setupGalaxy()
    {
        galaxyQuadtree = std::make_unique<pg::Quadtree<entt::entity>>(pg::fBox{{-750, -750}, {1500, 1500}});
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::normal_distribution<float> d(0.0f, 200.0f);
        std::normal_distribution<float> star_size_dist(0.0075f, 0.0025f);

        // Random number generator setup
        std::discrete_distribution<> star_class_dist(pgOrbit::star_class_probabilities.cbegin(),
                                                     pgOrbit::star_class_probabilities.cend());

        auto dot_sprite = getGame().getResource<pg::Sprite>("../data/circle_05.png");

        for ([[maybe_unused]] auto i : std::ranges::iota_view{0, 15000})
        {
            // color state
            auto rendererStates = pg::States{};
            auto color = pg::Color{255, 0, 0, 255};
            rendererStates.push(pg::TextureColorState{color});
            auto new_pos = pg::fVec2{d(gen), d(gen)};

            auto spectral_type = pgOrbit::indexToSpectralType(star_class_dist(gen));
            auto gammaCorrectedBrightness = pgOrbit::convertBrightnessByGamma(pgOrbit::perceivedBrightness, 2.6f);
            auto new_size =
                gammaCorrectedBrightness[magic_enum::enum_integer(spectral_type)] * pg::fVec2{1.0f, 1.0f} * 0.025f;
            auto entity = pg::game::makeEntity<pg::Transform2D,
                                               pg::game::Drawable,
                                               galaxy::StarSystemState,
                                               galaxy::Faction,
                                               pg::game::RenderState,
                                               pg::tags::GalaxyRenderTag>
                //
                (getGlobalRegistry(),
                 {.pos{new_pos}, .scale{new_size}, .scaleSpace{pg::TransformScaleSpace::Local}},
                 pg::game::Drawable{dot_sprite},
                 galaxy::StarSystemState{.spectralType{spectral_type}},
                 galaxy::Faction{"None"},
                 {std::move(rendererStates)},
                 {});

            galaxyQuadtree->insert({new_pos, new_size}, entity, galaxyQuadtree->root);
        }
        // add some background
        auto background_sprite = getGame().getResource<pg::Sprite>("../data/background/milky_way_blurred.png");
        auto states = pg::States{};
        states.push(pg::TextureAlphaState{static_cast<uint8_t>(galaxyConfig.background.opacity * 255)});
        pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::game::RenderState, pg::tags::GalaxyRenderTag>(
            getGlobalRegistry(),
            {.pos{0, 0}, .scale{0.5, 0.5}, .scaleSpace{pg::TransformScaleSpace::World}},
            pg::game::Drawable{background_sprite},
            {},
            {});

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
                getSceneRegistry(), {.pos{}, .scale{1, 1}}, pg::game::Drawable{box_prim}, {});
        }
    }

    void setupSelectionMarker()
    {
        auto marker =
            getGame().getResource<pg::Sprite, sdl::Renderer&>("../data/reticle.png", getGame().getApp().getRenderer());
        entt::entity markers =
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::game::RenderState, pg::tags::MarkerTag>(
                getSceneRegistry(), {.pos{0, 0}, .scale{0.015f, 0.015f}}, pg::game::Drawable{marker}, {}, {});
        addSingleton_as<entt::entity>("galaxy.debug.marker", markers);
    }

private:
    std::unique_ptr<pg::Quadtree<entt::entity>> galaxyQuadtree;
    config::Galaxy                              galaxyConfig;
    bool                                        isDragging{};
};

} // namespace galaxy