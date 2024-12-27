#pragma once
#include <string>

namespace galaxy {

struct BackgroundMusic
{
    std::vector<std::string> music_list;
    size_t                   next_index{0};
    bool                     loop{true};
    bool                     is_playing{false};

    std::string next()
    {
        if (music_list.empty() || next_index == std::numeric_limits<size_t>::max())
        {
            next_index = std::numeric_limits<size_t>::max();
            return "";
        }
        auto res = music_list.at(next_index);
        next_index++;
        if (next_index >= music_list.size())
        {
            if (loop) { next_index = 0; }
            else { next_index = std::numeric_limits<size_t>::max(); }
        }

        return res;
    }

    bool hasNext() const { return !music_list.empty() && next_index != std::numeric_limits<size_t>::max(); }
};

struct EventSound
{
    std::string sound_file;
};

struct SceneSoundScape
{
    BackgroundMusic                             background_music{};
    std::unordered_map<std::string, EventSound> event_sounds;
    float                                       master_volume{0.75f};
};

} // namespace galaxy