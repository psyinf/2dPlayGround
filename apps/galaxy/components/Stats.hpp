#pragma once
#include <unordered_map>

namespace galaxy {
struct FactionStatsValues
{
    uint64_t numDronesFailed{};
    uint64_t numDronesCreated{};
};

using FactionStats = std::unordered_map<std::string, FactionStatsValues>;

struct Stats
{
    FactionStats factionStats;
};

} // namespace galaxy