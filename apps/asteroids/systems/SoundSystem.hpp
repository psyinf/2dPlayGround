#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

namespace soundEngineX {
class SoundEngine;
class BackgroundPlayerInterface;
} // namespace soundEngineX

namespace asteroids {

using entt::literals::operator""_hs;

namespace events {
struct LaserFired;
struct Collision;
} // namespace events

class SoundSystem : public pg::game::SystemInterface
{
public:
    SoundSystem(pg::game::Game& game);
    void setup();

    void handle(const pg::game::FrameStamp& frameStamp);

private:
    void onLaserFired(const events::LaserFired& event);
    void onCollision(const events::Collision& event);

    std::unique_ptr<soundEngineX::SoundEngine>               _soundEngine;
    std::unique_ptr<soundEngineX::BackgroundPlayerInterface> _bgPlayer;
};

} // namespace asteroids