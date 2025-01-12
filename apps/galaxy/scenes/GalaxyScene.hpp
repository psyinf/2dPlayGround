#pragma once
#include <pgGame/core/Scene.hpp>
#include <memory>

#include <pgEngine/core/LoadSave.hpp>

#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/VecUtils.hpp>

#include <pgOrbit/StarParameters.hpp>
#include <systems/UpdateStarsSystem.hpp>
#include <systems/PickingSystem.hpp>
#include <components/Tags.hpp>

#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/components/singletons/RegisteredPreloaders.hpp>
#include <gui/MainFrameWidget.hpp>
#include <pgEngine/generators/markov/MarkovFrequencyMapSerializer.hpp>
#include <pgEngine/generators/MarkovNameGen.hpp>
#include <pgGame/events/SceneManagementEvents.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class CachedNames
{
public:
    CachedNames(const std::string&                        set_name,
                uint32_t                                  num_names,
                const pg::generators::MarkovFrequencyMap& mfm,
                std::function<void(float)>                progress)
    {
        auto file_name = fmt::format("../data/text/{}-{}.names", set_name, num_names);
        progress(0.0f);
        try
        {
            load(file_name);
            progress(1.0f);
        }
        catch (const std::exception&)
        {
            spdlog::info("No names loaded from file");
            generate(mfm, num_names, progress);
            save(file_name);
        }
    }

    void generate(const pg::generators::MarkovFrequencyMap& mfm,
                  size_t                                    numNames,
                  std::function<void(float)>                progress,
                  uint32_t                                  seed = 0)
    {
        _names.clear();
        _names.reserve(numNames);
        for (auto i : std::ranges::iota_view{0u, numNames})
        {
            progress(static_cast<float>(i) / numNames - 1);
            pg::SeedGenerator seed_gen(i + seed);

            auto name = pg::generators::markov::generate(3, 8, mfm, seed_gen);
            _names.push_back(name);
        }
        fmt::print("Generated {} names\n", numNames);
    }

    const std::string& getName(size_t index) const { return _names.at(index); }

    // serialize
    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(_names);
    }

    void save(const std::string& filename)
    {
        std::ofstream ofs(filename, std::ios::binary);
        // binary archive
        cereal::BinaryOutputArchive archive(ofs);
        archive(_names);
    }

    void load(const std::string& filename)
    {
        std::ifstream ifs(filename, std::ios::binary);
        // binary archive
        cereal::BinaryInputArchive archive(ifs);
        archive(_names);
    }

private:
    std::vector<std::string> _names;
};

class GalaxyScene : public pg::game::Scene
{
public:
    GalaxyScene(pg::game::Game& game, pg::game::SceneConfig&& scene_cfg)
      : pg::game::Scene(game, std::move(scene_cfg))
    {
        initialize();
    }

    void initialize()
    {
        auto& preLoaders =
            getGame().getSingleton<pg::singleton::RegisteredLoaders>(getSceneConfig().scene_id + ".loaders");
        // for now, no images
#if 0
        TODO: images can be loaded, but not added to the SDL in a background task.Doing so will interfere with the state while rendering.
            This needs a two stage approach where adding to SDL is done protected by a mutex
            std::vector<std::string> files = {
                "../data/reticle.png", "../data/background/milky_way_blurred.png", "../data/circle_05.png" };

        for (auto& file : files)
        {
            if (preLoaders.loaders.contains(file)) { continue; }
            auto loader = [&game, file](PercentCompleted& percentLoaded) {
                percentLoaded[file] = 0.0f;
                game.getResourceManager().get().load<pg::Sprite, sdl::Renderer&>(file, game.getApp().getRenderer());
                percentLoaded[file] = 1.0f;
                };
            preLoaders.loaders.emplace(file, loader);
        }
#endif
        getGame().addSingleton_as<std::atomic_bool>("markovFrequencyMap.loaded", false);
        /// add markov chain for names
        auto markov_loader = [this](PercentCompletedMap& completion) {
            // try cached
            // get atomic bool

            auto& loaded = getGame().getSingleton<std::atomic_bool>("markovFrequencyMap.loaded");
            pg::generators::MarkovFrequencyMap fmg;
            try
            {
                fmg = pg::generators::loadFrequencyMap("../data/text/star_names.mfm");
            }
            catch (const std::exception& /*e*/)
            {
                // log
                spdlog::info("No markov frequency map found for stars ("
                             "../data/text/star_names.mfm"
                             ")");
                std::ifstream fileStreamIn("../data/text/corpi/stars.txt", std::ios_base::binary);
                std::ifstream fileStreamIn2("../data/text/corpi/boys.txt", std::ios_base::binary);
                auto          file1_size = std::filesystem::file_size("../data/text/corpi/stars.txt");
                auto          file2_size = std::filesystem::file_size("../data/text/corpi/boys.txt");
                auto          total_size = file1_size + file2_size;
                auto          resource = "markov";
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
                pg::generators::saveFrequencyMap("../data/text/star_names.mfm", fmg.getFrequencyMap());
            }

            getGame().addSingleton_as<pg::generators::MarkovFrequencyMap>("markovFrequencyMap", fmg);
            loaded = true;
        };

        // cached name loader
        auto name_loader = [this](PercentCompletedMap& completion) {
            // wait for markov loader
            galaxy::config::Galaxy galaxy_config;
            galaxyConfig = pg::load<galaxy::config::Galaxy>("../data/galaxy_config.json", galaxy_config);
            completion["Generate Names"] = 0.0f;
            auto& loaded = getGame().getSingleton<std::atomic_bool>("markovFrequencyMap.loaded");
            // wait for markov loader
            while (!loaded)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            CachedNames cachedNames("star_names",
                                    galaxyConfig.creation.num_stars,
                                    getGame().getSingleton<pg::generators::MarkovFrequencyMap>("markovFrequencyMap"),
                                    [&completion](float percentage) { completion["Generate Names"] = percentage; });
            getGame().addSingleton_as<CachedNames>("cachedNames", cachedNames);
            completion["Generate Names"] = 1.0f;
        };

        preLoaders.loaders.emplace("Generate Markov Name Chains", std::move(markov_loader));
        preLoaders.loaders.emplace("Pre-load names", std::move(name_loader));
    }

    virtual ~GalaxyScene() = default;

    void start() override
    {
        if (!started())
        {
            galaxy::config::Galaxy galaxy_config;
            galaxyConfig = pg::load<galaxy::config::Galaxy>("../data/galaxy_config.json", galaxy_config);
            setupKeyHandler();
            setupOverlay();
            setupSelectionMarker();
            setupGalaxy();

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

        getKeyStateMap().registerMouseDoubleClickedCallback([this](auto /*pos*/, auto button, bool pressed) {
            if (!pressed && button == SDL_BUTTON_LEFT)
            {
                auto selected_entity = getGame().getSingleton_or<PickedEntity>("picked.entity", PickedEntity{}).entity;
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
            {std::make_unique<galaxy::gui::MainFrameWidget>(getGame()), pg::game::DRAWABLE_OVERLAY_MENU});
        /*
        pg::game::makeEntity<pg::game::GuiDrawable>(
            getSceneRegistry(),
            {std::make_unique<galaxy::gui::DashBoardWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<galaxy::gui::SystemInfoWidget>(getGame())});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<galaxy::gui::StatsWidget>(getGame())});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<galaxy::gui::MainBarWidget>(getGame())});

        pg::game::makeEntity<pg::game::GuiDrawable>(
            getSceneRegistry(),
            {std::make_unique<galaxy::gui::InSceneOptionsWidget>(getGame()), pg::game::DRAWABLE_OVERLAY_MENU});


            */
    }

    void setupGalaxy()
    {
        setupBackground();
        setupMapMarkers();
        setupStars();
    }

    void setupStars()
    {
        galaxyQuadtree = std::make_unique<pg::Quadtree<entt::entity>>(pg::fBox{{-750, -750}, {1500, 1500}});
        std::normal_distribution<float> d(0.0f, 200.0f);

        // Random number generator setup
        std::discrete_distribution<> star_class_dist(pgOrbit::star_class_probabilities.cbegin(),
                                                     pgOrbit::star_class_probabilities.cend());

        auto  dot_sprite = getGame().getResource<pg::Sprite>("data/circle_05.png");
        auto& gen = pg::SeedGenerator(galaxyConfig.creation.stars_seed).get();
        auto& cachedNames = getGame().getSingleton<CachedNames>("cachedNames");

        for ([[maybe_unused]] auto i : std::ranges::iota_view{0u, galaxyConfig.creation.num_stars})
        {
            // color state
            auto rendererStates = pg::States{};
            auto color = pg::Color{255, 0, 0, 255};
            rendererStates.push(pg::TextureColorState{color});
            rendererStates.push(pg::BlendModeState{SDL_BLENDMODE_ADD});

            auto new_pos = pg::fVec2{d(gen), d(gen)};

            auto spectral_type = pgOrbit::indexToSpectralType(star_class_dist(gen));
            auto gammaCorrectedBrightness = pgOrbit::adaptBrightnessByGamma(pgOrbit::perceivedBrightness, 2.6f);
            auto new_size =
                gammaCorrectedBrightness[magic_enum::enum_integer(spectral_type)] * pg::fVec2{1.0f, 1.0f} * 0.025f;

            auto entity = getGlobalRegistry().create();

            pg::game::addComponents<pg::Transform2D,
                                    pg::game::Drawable,
                                    galaxy::StarSystemState,
                                    galaxy::Faction,
                                    pg::game::RenderState,
                                    pg::tags::GalaxyRenderTag>
                //
                (getGlobalRegistry(),
                 entity,
                 {.pos{new_pos}, .scale{new_size}, .scaleSpace{pg::TransformScaleSpace::Local}},
                 pg::game::Drawable{dot_sprite},
                 galaxy::StarSystemState{.name = cachedNames.getName(i), .spectralType{spectral_type}},
                 galaxy::Faction{"None"},
                 {std::move(rendererStates)},
                 {});

            galaxyQuadtree->insert({new_pos, new_size}, entity, galaxyQuadtree->root);
        }
        addSingleton_as<const pg::Quadtree<entt::entity>&>("galaxy.quadtree", *galaxyQuadtree);
    }

    void setupMapMarkers()
    {
        auto rendererStates = pg::States{};
        auto color = pg::Color{0, 0, 255, 255};
        rendererStates.push(pg::ColorState{color});
        rendererStates.push(pg::BlendModeState{SDL_BLENDMODE_NONE});
        auto markers_group = std::make_shared<pg::Switch>();
        for (auto i : std::ranges::iota_view{0, 12})
        {
            auto angle = pg::math::toRadians(-15.0f + 30.0f * i);

            auto sector_pie = std::make_shared<pg::CircleSector>(
                pg::fVec2{0, 0}, 750.0f, angle, angle + pg::math::toRadians(30.0f), 25);
            auto inner_sector_pie = std::make_shared<pg::CircleSector>(
                pg::fVec2{0, 0}, 250.f, angle, angle + pg::math::toRadians(30.0f), 25, 500.0f);
            auto medium_sector_pie = std::make_shared<pg::CircleSector>(
                pg::fVec2{0, 0}, 500.f, angle, angle + pg::math::toRadians(30.0f), 25, 500.0f);

            auto group = std::make_shared<pg::Group>();
            group->addPrimitive(sector_pie);
            group->addPrimitive(inner_sector_pie);
            group->addPrimitive(medium_sector_pie);
            markers_group->addPrimitive(group);
        }

        auto entity =
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::game::RenderState, pg::tags::GalaxyRenderTag>(
                getGlobalRegistry(),
                {.pos{}, .scale{1, 1}, .scaleSpace{pg::TransformScaleSpace::World}},
                pg::game::Drawable{markers_group},
                {std::move(rendererStates)},
                {});

        auto setOpacity = [entity, color, &registry = getGame().getGlobalRegistry()](float opacity) mutable {
            auto& renderState = registry.get<pg::game::RenderState>(entity);

            auto colorState = renderState.states.get<pg::ColorState>();
            auto new_color = pg::vec_cast<uint8_t>(pg::vec_cast<float>(color) * opacity);

            colorState->setColor(new_color);
        };
        auto getOpacity = [color, &registry = getGame().getGlobalRegistry(), entity]() {
            auto& renderState = registry.get<pg::game::RenderState>(entity);
            return renderState.states.get<pg::ColorState>()->getColor()[3] / 255.0f;
        };
        registerAccessor<float>("galaxy.grid.opacity", std::move(setOpacity), std::move(getOpacity));
    }

    void setupBackground()

    {
        // add background
        auto background_sprite = getGame().getResource<pg::Sprite>("data/background/milky_way_blurred.png");
        auto states = pg::States{};
        states.push(pg::TextureAlphaState{static_cast<uint8_t>(galaxyConfig.background.opacity * 255)});
        states.push(pg::TextureColorState{pg::Color{255, 255, 255, 255}});
        states.push(pg::TextureBlendModeState{SDL_BLENDMODE_ADD});
        auto entity =
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, pg::game::RenderState, pg::tags::GalaxyRenderTag>(
                getGlobalRegistry(),
                {.pos{0.0f, 0.0f}, .scale{0.5f, 0.5f}, .scaleSpace{pg::TransformScaleSpace::World}},
                pg::game::Drawable{background_sprite},
                {states},
                {});

        // lambda to modify the background opacity
        auto setOpacity = [entity, &registry = getGame().getGlobalRegistry()](float opacity) mutable {
            auto& renderState = registry.get<pg::game::RenderState>(entity);
            renderState.states.replace<pg::TextureAlphaState>(static_cast<uint8_t>(opacity * 255));
        };
        auto getOpacity = [&registry = getGame().getGlobalRegistry(), entity]() {
            auto& renderState = registry.get<pg::game::RenderState>(entity);
            return renderState.states.get<pg::TextureAlphaState>()->getAlpha() / 255.0f;
        };
        registerAccessor<float>("galaxy.background.opacity", std::move(setOpacity), std::move(getOpacity));
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
            getGame().getResource<pg::Sprite, sdl::Renderer&>("data/reticle.png", getGame().getApp().getRenderer());
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