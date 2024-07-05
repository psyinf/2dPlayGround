#include "SoundSystem.hpp"
#include <pgGame/core/Game.hpp>
#include <events/LaserFired.h>
#include <events/Collision.h>

#include <sndX/BackgroundPlayer.hpp>
#include <sndX/SoundEngine.hpp>
#include <pgEngine/math/Random.hpp>

asteroids::SoundSystem::~SoundSystem() = default;

asteroids::SoundSystem::SoundSystem(pg::game::Game& game)
  : SystemInterface(game)
  , _soundEngine(std::make_unique<soundEngineX::SoundEngine>())
  , _bgPlayer(std::make_unique<soundEngineX::BackgroundPlayer>())
{
    game.getDispatcher().sink<events::LaserFired>().connect<&SoundSystem::onLaserFired>(this);
    game.getDispatcher().sink<events::Collision>().connect<&SoundSystem::onCollision>(this);
}

void asteroids::SoundSystem::setup() {}

void asteroids::SoundSystem::handle(const pg::game::FrameStamp&) {}

void asteroids::SoundSystem::onLaserFired(const events::LaserFired&)
{
    auto pitch = pg::randomBetween(0.95f, 1.15f);
    _bgPlayer->play("../data/sound/asteroids/laser_short.wav", {.pitch = pitch});
}

void asteroids::SoundSystem::onCollision(const events::Collision&)
{
    // TODO: the soundsystem should not directly act on the collision itself but on the effect
    //  e.g. the sound of the explosion of the asteroid, damage to the ship, etc.
    //
    //  TODO: play different sounds based on the type of collision
    //  TODO: variations of the same sound
    //  TODO: volume based on the size of the asteroid
    auto gain = pg::randomBetween(0.5f, 1.0f);
    auto pitch = pg::randomBetween(0.85f, 1.2f);
    _bgPlayer->play("../data/sound/asteroids/hit_comp.wav", {.pitch = pitch, .gain = gain});
}
