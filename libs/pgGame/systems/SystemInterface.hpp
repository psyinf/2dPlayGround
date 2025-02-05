#pragma once
#include <pgEngine/core/FrameStamp.hpp>
#include <unordered_map>
#include <pgGame/core/InputEventDispatcher.hpp>

namespace pg::game {
class Game;

class SystemInterface
{
public:
    struct Config
    {
        std::unordered_map<std::string, std::string> config;

        Config() = default;

        Config(std::initializer_list<std::pair<std::string, std::string>> list);

        bool has(const std::string& key) const;

        bool isSet(const std::string& key) const;
    };

    SystemInterface(Game& game, const std::string& name);

    virtual ~SystemInterface() = default;
    virtual void setup(std::string_view scene_id) = 0;
    virtual void handle(const FrameStamp& frameStamp) = 0;

    virtual void enterScene([[maybe_unused]] std::string_view newScene);

    virtual void exitScene([[maybe_unused]] std::string_view newScene);

    const Config& getConfig() const;

protected:
    std::shared_ptr<pg::InputEventHandlerInterface> _inputHandler;
    Game&                                           _game;
    std::string                                     _currentScene;
    std::string                                     _name;
};
} // namespace pg::game
