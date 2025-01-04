#pragma once
#include <Config.hpp>
#include <nlohmann/json.hpp>
#include <pgEngine/math/VecSerializer.hpp>

namespace galaxy::config {

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Star, default_color);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BackgoundParameters, opacity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ZoomParameters, min, max, factor)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StartingParameters, start_cycle, num_start_drones)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Creation, stars_seed, num_stars);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DroneParameters,
                                                max_acceleration,
                                                max_speed,
                                                max_range,
                                                expected_lifetime,
                                                reliability_factor)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Faction, name, color, startParams, droneParams)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Galaxy, background, zoom, creation, factions);
} // namespace galaxy::config