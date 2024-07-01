#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

namespace soundEngineX {
class SoundEngine;
class BackgroundPlayer;
} // namespace soundEngineX

namespace galaxy {

using entt::literals::operator""_hs;

class SoundSystem : public pg::game::SystemInterface
{
public:
    SoundSystem(pg::game::Game& game);
    void setup() override;

    void handle(const pg::game::FrameStamp& frameStamp) override;

    void onSwitchScene(std::string_view newScene) override;

private:
    std::unique_ptr<soundEngineX::SoundEngine>      _soundEngine;
    std::unique_ptr<soundEngineX::BackgroundPlayer> _bgPlayer;
};

} // namespace galaxy