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
