#pragma once
#include <string>

namespace galaxy {
struct SceneSoundScape
{
    std::vector<std::string> background_music_list;
    size_t                   background_music_next_index{0};
    bool                     background_music_loop{true};
    bool                     is_playing{false};

    std::string next()
    {
        if (background_music_list.empty() || background_music_next_index == std::numeric_limits<size_t>::max())
        {
            background_music_next_index = std::numeric_limits<size_t>::max();
            return "";
        }
        auto res = background_music_list.at(background_music_next_index);
        background_music_next_index++;
        if (background_music_next_index >= background_music_list.size())
        {
            if (background_music_loop) { background_music_next_index = 0; }
            else { background_music_next_index = std::numeric_limits<size_t>::max(); }
        }

        return res;
    }

    bool hasNext() const
    {
        return !background_music_list.empty() && background_music_next_index != std::numeric_limits<size_t>::max();
    }
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