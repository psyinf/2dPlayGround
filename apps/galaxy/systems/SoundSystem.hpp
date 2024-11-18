#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

namespace soundEngineX {
class SoundEngine;
class BackgroundPlayer;
} // namespace soundEngineX
class Dispatcher;

namespace galaxy {

using entt::literals::operator""_hs;

class SoundSystem : public pg::game::SystemInterface
{
public:
    SoundSystem(pg::game::Game& game, const std::string& name);
    virtual ~SoundSystem();
    void setup(std::string_view scene_id) override;

    void handle(const pg::FrameStamp& frameStamp) override;

    void enterScene(std::string_view newScene) override;
    void exitScene(std::string_view oldScene) override;

private:
    std::unique_ptr<soundEngineX::SoundEngine>      _soundEngine;
    std::unique_ptr<soundEngineX::BackgroundPlayer> _bgPlayer;
    std::unique_ptr<Dispatcher>                     _dispatcher;
};

} // namespace galaxy