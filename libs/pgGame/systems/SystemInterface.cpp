#include "SystemInterface.hpp"
#include <pgGame/core/Game.hpp>

const pg::game::SystemInterface::Config& pg::game::SystemInterface::getConfig() const
{
    static Config default_config;
    if (_game.hasSingleton<const Config&>(_name + "." + _currentScene + ".config"))
    {
        return _game.getSingleton<const Config&>(_name + "." + _currentScene + ".config");
    }
    return default_config;
}

void pg::game::SystemInterface::exitScene([[maybe_unused]] std::string_view newScene)
{
    _currentScene = "";
}

void pg::game::SystemInterface::enterScene([[maybe_unused]] std::string_view newScene)
{
    _currentScene = newScene;
}

pg::game::SystemInterface::SystemInterface(Game& game, const std::string& name)
  : _game(game)
  , _name(name)
{
}

bool pg::game::SystemInterface::Config::isSet(const std::string& key) const
{
    if (auto it = config.find(key); it != config.end()) { return it->second == "true"; }
    return false;
}

bool pg::game::SystemInterface::Config::has(const std::string& key) const
{
    return config.contains(key);
}

pg::game::SystemInterface::Config::Config(std::initializer_list<std::pair<std::string, std::string>> list)
{
    for (const auto& [key, value] : list)
    {
        config[key] = value;
    }
}
