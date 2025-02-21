#pragma once

#include <array>
#include <pgOrbit/StarParameters.hpp>
#include <pgEngine/math/Trigonometric.hpp>
#include <pgEngine/core/Constants.hpp>
#include <magic_enum.hpp>

namespace galaxy {
enum class ColonizationStatus
{
    Unexplored, ///< No one has been here yet
    Planned,    ///< A probe has been sent to this system
    Explored,   ///< A probe has been here and sent back data
    Colonized,  ///< A probe has established production facilities and started building more probes
};

struct StarSystemState
{
    ColonizationStatus    colonizationStatus{ColonizationStatus::Unexplored};
    std::string           name;
    pgOrbit::SpectralType spectralType{pgOrbit::SpectralType::Unknown};

    std::vector<entt::entity> orbits;
};

// some helper functions
inline auto sectorNameFromAngle(float angle_deg) -> std::string
{ // 0° is up
    angle_deg += 90.0f;
    // adjust for sector 0 starting at -15 degrees
    angle_deg += 15.0f;

    // normalize angle to 0-360
    angle_deg = std::fmod(angle_deg + 360.0f, 360.0f);

    // every 30 degrees, starting at -15 degrees. First sector is alpha, last is omega

    // first sector is between -15 and +15
    auto sector = static_cast<int>(std::floor((angle_deg) / 30.0f));

    return pg::constants::greek_letter_names.at(sector);
}

// the remainder-angle inside a sector
inline auto inSectorAngle(float angle_deg) -> float
{
    return std::fmod(angle_deg + 15.0f, 30.0f);
}

inline auto buildSystemPositionStr(pg::fVec2 pos) -> std::string
{
    auto polar_pos = pg::math::toPolar(pos);

    auto sector_name = sectorNameFromAngle(pg::math::toDegrees(std::get<1>(polar_pos)));
    auto sector_angle = 10 * inSectorAngle(std::get<1>(polar_pos));
    // name is sector - angle_in_sector:distance
    return std::format("{}-{:.0f}:{:.0f}", sector_name, sector_angle, 10 * std::get<0>(polar_pos));
}
} // namespace galaxy