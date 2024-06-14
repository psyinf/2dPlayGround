#pragma once
#include <unordered_map>

namespace galaxy {
struct FactionStatsValues
{
    uint64_t numDronesFailed{};
    uint64_t numDronesCreated{};
    uint64_t systemsVisited{};
};

struct GalaxyStats
{
    uint64_t numSystems{};
};

using FactionStats = std::unordered_map<std::string, FactionStatsValues>;

struct Stats
{
    FactionStats factionStats;
    GalaxyStats  galaxyStats;
};

} // namespace galaxy