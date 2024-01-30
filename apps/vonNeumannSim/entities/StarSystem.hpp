#pragma once
#include <SDLVec.h>
namespace vNms {
enum class ColonizationStatus
{
    Unexplored, //< No one has been here yet
    Explored,   //< A probe has been here and sent back data
    Colonized,  //< A probe has established production facilities and started building more probes
};

struct StarSystem
{
    ColonizationStatus colonizationStatus;
    pg::Vec3d position;
};
} // namespace vNms