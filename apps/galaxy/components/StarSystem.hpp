#pragma once

#include <array>
#include <pgEngine/math/Vec.hpp>

namespace galaxy {
enum class ColonizationStatus
{
    Unexplored, //< No one has been here yet
    Planned,    //< A probe has been sent to this system
    Explored,   //< A probe has been here and sent back data
    Colonized,  //< A probe has established production facilities and started building more probes
};

enum class SpectralType
{
    O,          //< Blue
    B,          //< Blue-white
    A,          //< White
    F,          //< Yellow-white
    G,          //< Yellow
    K,          //< Orange
    M,          //< Red
    BrownDwarf, //< Brown dwarf
    WhiteDwarf, //< White dwarf
    BlackHole,
    NeutronStar,
    Unknown,
};

static constexpr auto StarColors = std::array<pg::Color, 7>{
    pg::Color{155, 176, 255}, // O: Bluish white
    pg::Color{170, 191, 255}, // B: Blue-white
    pg::Color{202, 215, 255}, // A: White
    pg::Color{248, 247, 255}, // F: Yellow-white
    pg::Color{255, 244, 234}, // G: Yellow
    pg::Color{255, 210, 161}, // K: Orange
    pg::Color{255, 204, 111}  // M: Reddish-orange
};

struct Orbit
{
    entt::entity orbiting_entity{entt::null};
    float        distance{0.0f};
    float        eccentricity{0.0f};
    float        inclination{0.0f};
    float        longitudeOfAscendingNode{0.0f};
    float        argumentOfPeriapsis{0.0f};
    float        meanAnomaly{0.0f};
};

struct StarSystemState
{
    ColonizationStatus colonizationStatus{ColonizationStatus::Unexplored};
    std::string        name;
    SpectralType       spectralType{SpectralType::Unknown};

    std::vector<entt::entity> orbits;
};

} // namespace galaxy