#pragma once

namespace pg::game {

struct VFSConfig
{
    enum class VFSType
    {
        PHYSICAL,
        ZIP,
        MEMORY
    };
    using Root = std::string;
    using Alias = std::string;

    Root    root;
    Alias   alias;
    VFSType type{VFSType::PHYSICAL};
};

struct GameConfig
{
    GenericConfig            options;
    pg::config::WindowConfig windowConfig;
    std::vector<VFSConfig>   vfsConfigs;
    std::string              resourcePrefix;

    SDL_Rect getWindowRect()
    {
        return {windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]};
    }
};

} // namespace pg::game
