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
    uint8_t  num_start_drones{1};
};

struct DroneParameters
{
    float    max_acceleration{0.1f};
    float    max_speed{1.0f};
    float    max_range{40.0f};
    uint32_t expected_lifetime{10000};
    float    reliability_factor{1.0};
};

struct Faction
{
    std::string        name;
    pg::Color          color{0, 0, 255, 255};
    StartingParameters startParams;
    DroneParameters    droneParams;
};

struct BackgoundParameters
{
    float opacity{0.1f};
};

struct ZoomParameters
{
    float min{0.5f};
    float max{50.0f};
    float factor{0.1f};
};

struct Debugging
{
    bool draw_debug_items{false};
    bool draw_quadtree{false};
};

struct Galaxy
{
    Star                 star;
    BackgoundParameters  background;
    ZoomParameters       zoom;
    Debugging            debugging;
    uint32_t             stars_seed{0};
    uint32_t             num_stars{15000};
    std::vector<Faction> factions = {
        {.name{"Terra"}, .color{0, 0, 255, 255}},
        //{.name{"Vogons"}, .color{255, 0, 0, 255}},
        //{.name{"Grays"}, .color{255, 255, 0, 255}},
        //{.name{"Vogons"}, .color{255, 0, 0, 255}},
        //{.name{"Grays"}, .color{255, 255, 0, 255}},
        //{.name{"Drifters"}, .color{0, 255, 255, 255}, .startParams{.start_cycle{}, .num_start_drones{}}}
        //
    };
};

}} // namespace galaxy::config