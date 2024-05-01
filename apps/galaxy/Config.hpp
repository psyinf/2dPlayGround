#pragma once

#include <pgEngine/math/Vec.hpp>
#include <vector>
#include <string>

namespace galaxy { namespace config {
struct Star
{
    pg::Color default_color{0x90, 0x90, 0x70, 12};
};

struct Faction
{
    std::string name;
    pg::Color   color{0, 0, 255, 255};
};

struct Galaxy
{
    Star star;

    std::vector<Faction> factions = {{.name{"Terra"}, .color{0, 0, 255, 255}}};
};
}} // namespace galaxy::config
