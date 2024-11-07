#pragma once
#include <string>

namespace galaxy {  
struct SceneSoundScape
{
    //TODO: generic resource
    std::string background_music;
};


struct SoundConfig
{
    float master_gain{1.0f};
//     float music_gain{1.0f};
//     float sfx_gain{1.0f};
//     float voice_gain{1.0f};
//     float ambiance_gain{1.0f};
//     float ui_gain{1.0f};
};

;

} // namespace galaxy