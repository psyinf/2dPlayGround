#pragma once
#include <pgGame/core/FrameStamp.hpp>
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

        Config(std::initializer_list<std::pair<std::string, std::string>> list)
        {
            for (const auto& [key, value] : list)
            {
                config[key] = value;
            }
        }

        bool has(const std::string& key) const { return config.contains(key); }

        bool isSet(const std::string& key) const
        {
            if (auto it = config.find(key); it != config.end()) { return it->second == "true"; }
            return false;
        }
    };

    SystemInterface(Game& game, const std::string& name)
      : _game(game)
      , _name(name)
    {
    }

    virtual ~SystemInterface() = default;
    virtual void setup() = 0;
    virtual void handle(const FrameStamp& frameStamp) = 0;

    virtual void enterScene([[maybe_unused]] std::string_view newScene) { _currentScene = newScene; }

    virtual void exitScene([[maybe_unused]] std::string_view newScene) { _currentScene = ""; }

    const Config& getConfig() const;

protected:
    std::shared_ptr<pg::InputEventHandlerInterface> _inputHandler;
    Game&                                           _game;
    std::string                                     _currentScene;
    std::string                                     _name;
};
} // namespace pg::game
