#pragma once
#include <components/SoundScape.hpp>

namespace galaxy {

struct SceneSoundScapeConfig
{
    std::vector<std::string> background_music_list;
    bool                     background_music_loop{true};

    void toSoundScape(SceneSoundScape& soundScape) const
    {
        soundScape.background_music_list = background_music_list;
        soundScape.background_music_loop = background_music_loop;
    }
};

struct SceneConfig
{
    SceneSoundScapeConfig soundScape;
};

// a configuration map holding all configurations
//   current idea: each system will fill the map with its own configuration
//

} // namespace galaxy
