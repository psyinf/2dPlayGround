#pragma once

namespace pg::game {

struct VFSConfig
{
    using Root = std::string;
    using Alias = std::string;

    Root  root;  // root path
    Alias alias; // alias/mount point
};

struct GameConfig
{
    GenericConfig            options;
    pg::config::WindowConfig windowConfig;
    std::vector<VFSConfig>   vfsConfigs;

    SDL_Rect getWindowRect()
    {
        return {windowConfig.offset[0], windowConfig.offset[1], windowConfig.size[0], windowConfig.size[1]};
    }
};

} // namespace pg::game
