#pragma once
#include <pgGame/core/Game.hpp>
#include <components/Faction.hpp>
#include <ranges>

namespace galaxy {

static const auto& getFactionConfig(pg::game::Game& game, const Faction& faction)
{
    const auto& galaxy_config = game.getSingleton<const galaxy::config::Galaxy&>("galaxy.config");
    // find by faction name
    auto iter =
        std::ranges::find_if(galaxy_config.factions, [&faction](const auto& f) { return f.name == faction.name; });
    // get drone params
    if (iter == galaxy_config.factions.end()) { throw std::logic_error("Drone with unknown faction"); }

    return *iter;
}
} // namespace galaxy