#include "SoundSystem.hpp"
#include <pgGame/core/Game.hpp>

#include <sndX/BackgroundPlayer.hpp>
#include <sndX/SoundEngine.hpp>
#include <pgEngine/math/Random.hpp>

galaxy::SoundSystem::SoundSystem(pg::game::Game& game)
  : SystemInterface(game)
  , _soundEngine(std::make_unique<soundEngineX::SoundEngine>())
  , _bgPlayer(std::make_unique<soundEngineX::BackgroundPlayer>())
{
}

void galaxy::SoundSystem::setup() {}

void galaxy::SoundSystem::handle(const pg::game::FrameStamp&) {}

void galaxy::SoundSystem::onSwitchScene(std::string_view newScene)
{
    // TODO: scene soundscape
    if (newScene == "galaxy")
    {
        //       _bgPlayer->play("../data/sound/galaxy/ambient.wav", {.loop = true});
    }
}