#pragma once
#include <pgEngine/math/Vec.hpp>

namespace galaxy {
enum class ColonizationStatus
{
    Unexplored, //< No one has been here yet
    Explored,   //< A probe has been here and sent back data
    Colonized,  //< A probe has established production facilities and started building more probes
};

struct StarSystemState
{
    ColonizationStatus colonizationStatus;
};
} // namespace galaxy