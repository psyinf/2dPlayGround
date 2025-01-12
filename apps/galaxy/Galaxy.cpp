#include "Galaxy.hpp"
#include <scenes/GalaxyScene.hpp>
#include <scenes/MainMenuScene.hpp>

#include <scenes/LoadResourcesScene.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/SoundSystem.hpp>
#include <systems/GuiRenderSystem.hpp>
#include <systems/UpdateCurrentSystem.hpp>
#include <scenes/SystemScene.hpp>

#include <systems/DroneSystem.hpp>
#include <systems/LifetimeSystem.hpp>
#include <systems/BehaviorSystem.hpp>
#include <systems/StatsSystem.hpp>

galaxy::GalacticCore::GalacticCore()
  : game(std::make_unique<pg::game::Game>())
{
}

struct Resource
{
    std::string name;
    std::string path;
};

void galaxy::GalacticCore::setup()
{
    // TODO: from file

    static const auto event_sound_cfg = std::unordered_map<std::string, EventSound>{
        {"PickEvent", {"data/sound/asteroids/laser_short.wav"}},
        {"MenuButtonPressed", {"data/sounds/ui/spacebar-click-keyboard-199448.mp3"}}};

    // TODO: structure of files/resources to be loaded
    //  the structure must be annotated to allow for dispatching to specific loader
    //   e.g. sounds, markov-chains, etc
    //  we also need some loader abstraction/way to report progress -> LoaderProgressInterface

    game->getConfig().addPerSceneConfig<galaxy::SceneSoundScape>(
        "mainMenu",
        "soundScape",
        {.background_music{.music_list{{"../data/music/dead-space-style-ambient-music-184793.mp3"}}},
         .event_sounds = {event_sound_cfg}});

    game->getConfig().addPerSceneConfig<galaxy::SceneSoundScape>(
        "galaxy",
        "soundScape",
        {.background_music{.music_list{{"data/music/a-meditation-through-time-amp-space-11947.mp3"}}},
         .event_sounds = {event_sound_cfg}

        });
    //     game->getConfig().addPerSceneConfig<galaxy::SceneSoundScape>(
    //         "loadGalaxy",
    //         "soundScape",
    //         {.background_music{.music_list{{.background_music{"../data/music/dead-space-style-ambient-music-184793.mp3"}}},
    //         .event_sounds = {event_sound_cfg}
    //
    //         });
    // systems
    pg::game::SystemsFactory::registerSystem<galaxy::SoundSystem>("soundSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::GuiRenderSystem>("guiSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::TaggedRenderSystem<pg::tags::GalaxyRenderTag>>(
        "galaxyRenderSystem", RenderSystemConfig{.perScene{false}});
    pg::game::SystemsFactory::registerSystem<galaxy::TaggedRenderSystem<pg::tags::MarkerTag>>(
        "galaxyMarkers", RenderSystemConfig{.perScene{true}, .clear{false}});

    pg::game::SystemsFactory::registerSystem<
        galaxy::TaggedRenderSystem<pg::tags::SystemRenderTag, pg::tags::SelectedItemTag>>("systemRenderSystem",
                                                                                          RenderSystemConfig{});

    pg::game::SystemsFactory::registerSystem<galaxy::UpdateStarsSystem>("updateStarsSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::UpdateCurrentSystem>("updateCurrentSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::PickingSystem>("pickingSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::DroneSystem>("droneSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::LifetimeSystem>("lifeTimeSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::BehaviorSystem>("behaviorSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::StatsSystem>("statsSystem");

    // scenes
    game->createScene<galaxy::MainMenuScene>(
        {.scene_id = "mainMenu", .systems = {"soundSystem", "guiSystem"}, .followUpScene = "galaxy"});
    game->createScene<galaxy::LoadResourcesScene>({.scene_id = "loadGalaxy", .systems = {"guiSystem"}}, "galaxy");
    game->createScene<galaxy::GalaxyScene>({.scene_id = "galaxy",
                                            .systems = {"soundSystem",
                                                        "galaxyRenderSystem",
                                                        "galaxyMarkers",
                                                        "guiSystem",
                                                        "updateStarsSystem",
                                                        "pickingSystem",
                                                        "droneSystem",
                                                        "lifeTimeSystem",
                                                        "behaviorSystem",
                                                        "statsSystem"}});

    game->createScene<galaxy::SystemScene>(
        {.scene_id = "system", .systems = {"soundSystem", "systemRenderSystem", "guiSystem", "updateCurrentSystem"}});

    game->addSingleton_as<pg::game::SystemInterface::Config>("guiSystem.loadGalaxy.config",
                                                             pg::game::SystemInterface::Config{{"standalone", "true"}});
    game->addSingleton_as<pg::game::SystemInterface::Config>(
        "guiSystem.system.config", pg::game::SystemInterface::Config{{"standalone", "false"}});

    auto& scene = game->switchScene("mainMenu");
    scene.start();
}

void galaxy::GalacticCore::run()
{
    game->loop();
}
