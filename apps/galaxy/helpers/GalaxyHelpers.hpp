#pragma once
#include <pgGame/core/Game.hpp>
#include <components/Faction.hpp>
#include <Config.hpp>
#include <ranges>

namespace galaxy {

static const auto& getFactionConfig(pg::game::Game& game, const std::string& faction_name)
{
    const auto& galaxy_config = game.getCurrentScene().getSingleton<const galaxy::config::Galaxy&>("galaxy.config");
    // find by faction name
    auto iter =
        std::ranges::find_if(galaxy_config.factions, [&faction_name](const auto& f) { return f.name == faction_name; });
    // get drone params
    if (iter == galaxy_config.factions.end()) { throw std::logic_error("Unknown faction"); }

    return *iter;
}
} // namespace galaxy