#include "SoundSystem.hpp"
#include <pgGame/core/Game.hpp>

#include <sndX/BackgroundPlayer.hpp>
#include <sndX/SoundEngine.hpp>
#include <sndX/Listener.hpp>
#include <pgf/taskengine/TaskEngine.hpp>
#include <pgEngine/math/Random.hpp>
#include <events/PickEvent.hpp>
#include <events/UIEvents.hpp>
#include <components/SoundScape.hpp>
#include <resources/SoundResource.hpp>
#include <pgFoundation/NamedTypeRegistry.hpp>
#include <pgGame/components/singletons/RegisteredPreloaders.hpp>

class Dispatcher
{
    using SoundEventMap = std::unordered_map<std::string, galaxy::EventSound>;

public:
    Dispatcher(soundEngineX::BackgroundPlayer& bgPlayer)
      : _bgPlayer(bgPlayer)
    {
    }

    void setSoundEventMap(const SoundEventMap& soundEventMap) { _soundEventMap = soundEventMap; }

    auto getSoundEventMap() -> SoundEventMap& { return _soundEventMap; }

    template <typename T>
    void dispatch(const T& /*event*/)
    {
        // do something with event
        const auto& name = _registry.getTypeName<T>();
        spdlog::info("Dispatching event {} with name {}", typeid(T).name(), name);

        try
        {
            auto& sound = _soundEventMap.at(name);
            _bgPlayer.play(sound.sound_file);
        }
        catch (const std::out_of_range&)
        {
            spdlog::error("Dispatcher: No sound for event '{}'", name);
        }
        catch (const std::exception& e)
        {
            spdlog::error(
                "Dispatcher: Error playing '{}'[{}] ({})", name, _soundEventMap.at(name).sound_file, e.what());
        }
    }

    template <typename T>
    bool hasSoundEvent() const
    {
        static T    t{}; // force instantiation of T, to get the type name
        const auto& name = _registry.getTypeName<T>();
        return _soundEventMap.contains(name);
    }

private:
    pgf::NamedTypeRegistry          _registry;
    soundEngineX::BackgroundPlayer& _bgPlayer;
    SoundEventMap                   _soundEventMap;
};

galaxy::SoundSystem::~SoundSystem() = default;

galaxy::SoundSystem::SoundSystem(pg::game::Game& game, const std::string& name)
  : SystemInterface(game, name)
  , _soundEngine(std::make_unique<soundEngineX::SoundEngine>())
  , _bgPlayer(std::make_unique<soundEngineX::BackgroundPlayer>())
  , _dispatcher(std::make_unique<Dispatcher>(*_bgPlayer))

{
}

void galaxy::SoundSystem::setup(std::string_view scene_id)
{
    // register-pre loaders
    auto& scene_config = _game.getSingleton<pg::game::SceneConfig>(scene_id);
#if 1
    // register all resources here
    auto& preLoaders = _game.getSingleton<pg::singleton::RegisteredLoaders>(std::string{scene_id} + ".loaders");
    std::vector<std::string> sound_files = {
        "../data/music/a-meditation-through-time-amp-space-11947.mp3",
        "../data/music/dead-space-style-ambient-music-184793.mp3",
        "../data/music/universe-cosmic-space-ambient-interstellar-soundscape-sci-fi-181916.mp3"};

    for (auto& file : sound_files)
    {
        if (preLoaders.loaders.contains(file)) { continue; }
        auto loader = [this, file](PercentCompleted& percentLoaded) {
            _game.getResourceManager().get().load<std::shared_ptr<soundEngineX::Buffer>, float&>(file,
                                                                                                 percentLoaded[file]);
        };
        preLoaders.loaders.emplace(file, loader);
    }
#endif
    auto& soundScapeConfig = _game.getConfig().getPerSceneConfig<SceneSoundScape>(std::string{scene_id}, "soundScape");
    _game.getCurrentScene().addSingleton_as<SceneSoundScape>("scene.soundScape", soundScapeConfig);
    // add sound events to dispatcher
    _dispatcher->setSoundEventMap(soundScapeConfig.event_sounds);

    if (_dispatcher->hasSoundEvent<galaxy::events::PickEvent>())
    {
        _game.getDispatcher()
            .sink<galaxy::events::PickEvent>()
            .connect<&Dispatcher::dispatch<galaxy::events::PickEvent>>(_dispatcher);
    }
    if (_dispatcher->hasSoundEvent<galaxy::events::MenuButtonPressed>())
    {
        _game.getDispatcher()
            .sink<galaxy::events::MenuButtonPressed>()
            .connect<&Dispatcher::dispatch<galaxy::events::MenuButtonPressed>>(_dispatcher);
    }
}

void galaxy::SoundSystem::handle(const pg::FrameStamp&)
{
    auto& soundScape = _game.getCurrentScene().getSingleton<SceneSoundScape>("scene.soundScape");

    if (!soundScape.background_music.is_playing && soundScape.background_music.hasNext())
    {
        _bgPlayer->play(soundScape.background_music.next(), {.loop = false}, [&soundScape]() {
            soundScape.background_music.is_playing = false;
        });
        soundScape.background_music.is_playing = true;
    }
}

void galaxy::SoundSystem::exitScene([[maybe_unused]] std::string_view oldScene) {}

void galaxy::SoundSystem::enterScene(std::string_view /*newScene*/)
{
    // TODO: config should decide if we stop all sounds or not
    _soundEngine->stopAll();
}