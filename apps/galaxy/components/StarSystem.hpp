#pragma once

namespace galaxy {
enum class ColonizationStatus
{
    Unexplored, //< No one has been here yet
    Planned,    //< A probe has been sent to this system
    Explored,   //< A probe has been here and sent back data
    Colonized,  //< A probe has established production facilities and started building more probes
};

struct StarSystemState
{
    ColonizationStatus colonizationStatus{ColonizationStatus::Unexplored};
};
} // namespace galaxy