#include "SoundSystem.hpp"
#include <pgGame/core/Game.hpp>

#include <sndX/BackgroundPlayer.hpp>
#include <sndX/SoundEngine.hpp>
#include <pgEngine/math/Random.hpp>
#include <events/PickEvent.hpp>
#include <events/UIEvents.hpp>

template <size_t N>
struct StringLiteral
{
    constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

    char value[N];
};

/*TODO : that's only compile time string literals.In order to have configurable sounds per event at runtime we need to
 * have a map of event anyways*/

class SoundDispatcher
{
    using SoundEventMap = std::unordered_map<std::string, std::string>;

public:
    SoundDispatcher(soundEngineX::BackgroundPlayer& bgPlayer, SoundEventMap&& soundEventMap)
      : _bgPlayer(bgPlayer)
      , _soundEventMap{soundEventMap}
    {
    }

    template <typename T, StringLiteral i>
    void play(const T& /*event*/)
    {
        _bgPlayer.play(_soundEventMap.at(i.value));
    }

    soundEngineX::BackgroundPlayer& _bgPlayer;
    SoundEventMap                   _soundEventMap;
};

galaxy::SoundSystem::~SoundSystem() = default;

galaxy::SoundSystem::SoundSystem(pg::game::Game& game)
  : SystemInterface(game)
  , _soundEngine(std::make_unique<soundEngineX::SoundEngine>())
  , _bgPlayer(std::make_unique<soundEngineX::BackgroundPlayer>())
{
    static SoundDispatcher soundDispatcher(
        *_bgPlayer,
        {{"pick", "../data/sound/asteroids/laser_short.wav"}, {"click", "../data/sounds/ui/spacebar-click-keyboard-199448.mp3"}});
    game.getDispatcher()
        .sink<galaxy::events::PickEvent>()
        .connect<&SoundDispatcher::play<galaxy::events::PickEvent, "pick">>(&soundDispatcher);
    game.getDispatcher()
        .sink<galaxy::events::MenuButtonPressed>()
        .connect<&SoundDispatcher::play<galaxy::events::MenuButtonPressed, "click">>(&soundDispatcher);
}

void galaxy::SoundSystem::setup() {}

void galaxy::SoundSystem::handle(const pg::game::FrameStamp&) {}

void galaxy::SoundSystem::exitScene([[maybe_unused]] std::string_view oldScene)
{
    _soundEngine->stopAll();
}

void galaxy::SoundSystem::enterScene(std::string_view newScene)
{
    // TODO: resources repo should be configurable regarding its target path
    // TODO: scene soundscape
    try
    {
        _soundEngine->stopAll();
        if (newScene == "splashScreen")
        {
            _bgPlayer->play("../data/music/dead-space-style-ambient-music-184793.mp3", {.loop = true});
        }
        else if (newScene == "galaxy")
        {
            _bgPlayer->play("../data/music/a-meditation-through-time-amp-space-11947.mp3", {.loop = true});
        }
    }
    catch (const std::exception& e)
    {
        spdlog::error("Failed to play sound {}", e.what());
    }
}