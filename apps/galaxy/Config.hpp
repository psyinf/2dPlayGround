#pragma once

#include <pgEngine/math/Vec.hpp>
#include <vector>
#include <string>

namespace galaxy { namespace config {
struct Star
{
    pg::Color default_color{0x90, 0x90, 0x70, 12};
};

struct StartingParameters
{
    uint64_t start_cycle{0};
    uint8_t  start_drones{1};
};

struct Faction
{
    std::string        name;
    pg::Color          color{0, 0, 255, 255};
    StartingParameters startParams;
};

struct Galaxy
{
    Star star;

    std::vector<Faction> factions = {
        {.name{"Terra"}, .color{0, 0, 255, 255}},
        {.name{"Vogons"}, .color{255, 0, 0, 255}},
        {.name{"Drifters"}, .color{0, 255, 255, 255}, .startParams{.start_cycle{}, .start_drones{0}}}
        //
    };
};

}} // namespace galaxy::config