#include "DroneSystem.hpp"
#include <entities/Drone.hpp>
#include <entities/StarSystem.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/VecOperations.hpp>
#include <Config.hpp>
#include <entities/Faction.hpp>
#include <pgEngine/math/Random.hpp>
#include <events/DroneEvents.hpp>

#include <ranges>
#include <pgGame/entities/RenderState.hpp>

void galaxy::DroneSystem::setup()
{
    game.getDispatcher().sink<galaxy::events::DroneFailedEvent>().connect<&galaxy::DroneSystem::handleDroneFailed>(
        *this);
}

void galaxy::DroneSystem::makeDrone(pg::fVec2 pos, galaxy::Faction faction)
{
    auto dot_sprite = game.getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");
    // get faction config
    const auto& galaxy_config = game.getSingleton<const galaxy::config::Galaxy&>("galaxy.config");
    // find by faction name
    auto iter =
        std::ranges::find_if(galaxy_config.factions, [&faction](const auto& f) { return f.name == faction.name; });
    // get drone params
    if (iter == galaxy_config.factions.end()) { throw std::logic_error("Drone with unknown faction"); }
    auto& drone_params = (*iter).droneParams;

    // create a drone
    auto renderState = pg::States{};
    renderState.push(pg::TextureColorState{faction.entityColor});
    auto entity = pg::game::makeEntity<pg::Transform2D,
                                       pg::game::Drawable,
                                       galaxy::Drone,
                                       galaxy::Dynamic,
                                       galaxy::Faction,
                                       pg::game::RenderState>(game.getRegistry(),
                                                              {.pos{pos}, .scale{0.00125, 0.00125}},
                                                              pg::game::Drawable{dot_sprite},
                                                              {Drone::fromConfig(drone_params)},
                                                              galaxy::Dynamic{},
                                                              std::move(faction),
                                                              {std::move(renderState)});

    game.getDispatcher().trigger<galaxy::events::DroneCreatedEvent>({entity, pos});
}

void galaxy::DroneSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    createFactions(frameStamp);
    // TODO statemachine (e.g. https://github.com/digint/tinyfsm)
    auto view = game.getRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Dynamic>();
    for (auto entity : view)
    {
        auto&& [drone, transform, dynamic] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Dynamic>(entity);
        drone.lifetime += 1;
        if (!checkForFailure(drone, entity)) { continue; }
        if (drone.atTarget)
        {
            if (!handleProduction(entity)) { continue; }
        }
        if (drone.hasTarget)
        {
            // travel to target
            // transform.pos += (drone.targetPos - transform.pos) * 0.01f;
            transform.pos = dynamic.calculateDynamics(
                vec_cast<float>(transform.pos), drone.targetPos, drone.maxAcceleration, drone.maxVelocity, 0.02f);
            // check if we are at the target
            if (lengthSquared(transform.pos - drone.targetPos) < 0.001f)
            {
                transform.pos = drone.targetPos;
                dynamic.reset();
                drone.hasTarget = false;
                drone.atTarget = true;
                drone.waitCycles = 100;
                // mark the star as visited
                auto& starsystem = game.getRegistry().get<galaxy::StarSystemState>(drone.targetId);
                starsystem.colonizationStatus = galaxy::ColonizationStatus::Explored;
                continue;
            }
            continue;
        }

        if (!drone.hasTarget)
        {
            if (!findNewTarget(entity)) { continue; }
        }
    }
}

void galaxy::DroneSystem::handleDroneFailed(galaxy::events::DroneFailedEvent& event)
{
    // later: decide if the drone dies or becomes a drifter
    // for now send a signal to destroy the entity
    game.getRegistry().destroy(event.entity);
}

void galaxy::DroneSystem::createFactions(const pg::game::FrameStamp& frameStamp)
{
    static std::random_device rd;
    static std::mt19937       gen{rd()};
    // setup for all factions
    auto galaxy_config = game.getSingleton<const galaxy::config::Galaxy&>("galaxy.config");

    for (const auto& faction : galaxy_config.factions)
    {
        if (faction.startParams.start_cycle != frameStamp.frameNumber) { continue; }

        auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>();
        auto it = view.begin();
        auto size = static_cast<size_t>(std::distance(view.begin(), view.end()));
        std::advance(it, pg::randomBetween<size_t>({0}, {size}));
        auto entity = *it;
        auto&& [drawable, transform, starsystem] =
            view.get<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>(entity);
        // get a single star system
        galaxy::Faction factionComponent{faction.name, faction.color, faction.color};
        for (auto num : std::ranges::iota_view{0u, faction.startParams.start_drones})
        {
            makeDrone(transform.pos, factionComponent);
        }
    }
}

bool galaxy::DroneSystem::checkForFailure(galaxy::Drone& drone, entt::entity entity)
{
    auto l = 1.0 - (std::exp(0.0000005f * drone.lifetime) - 1);

    if (pg::randomBetween(0.0f, 1.0f) > l)
    {
        // if we have a target, that is marked as planned, we need to unmark it
        if (drone.hasTarget)
        {
            auto& starsystem = game.getRegistry().get<galaxy::StarSystemState>(drone.targetId);
            // TODO: unmark only if planned (not explored)
            starsystem.colonizationStatus = galaxy::ColonizationStatus::Unexplored;
        }

        auto event = galaxy::events::DroneFailedEvent{entity};
        return false;
    }
    return true;
}

bool galaxy::DroneSystem::handleProduction(entt::entity entity)
{
    auto view = game.getRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Faction>();
    auto&& [drone, transform, faction] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Faction>(entity);

    drone.waitCycles -= 1;
    if (drone.waitCycles == 50) { makeDrone(transform.pos, faction); }
    if (drone.waitCycles == 0)
    {
        // get view
        drone.atTarget = false;
        drone.hasTarget = false;
        drone.targetId = entt::null;
        // build new drones
        // TODO: constant for replication factor
        makeDrone(transform.pos, faction);

        drone.waitCycles = 100;
        return true;
    }
    return false;
}

bool galaxy::DroneSystem::findNewTarget(entt::entity entity)
{
    auto& quadtree = game.getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
    auto  view = game.getRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Faction>();
    auto&& [drone, transform, faction] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Faction>(entity);

    auto r = quadtree.rangeQuery(pg::fBox::fromMidpoint(transform.pos, {drone.range * 0.5f, drone.range * 0.5f})) |
             std::views::filter([&transform](auto result) { return !equal(result.box.midpoint(), transform.pos); }) |
             // remove stars that are already visited
             std::views::filter([&game = game](auto result) {
                 auto& starsystem = game.getRegistry().get<galaxy::StarSystemState>(result.data.front());
                 return starsystem.colonizationStatus == galaxy::ColonizationStatus::Unexplored;
             });
    // to vector
    auto res = r | std::ranges::to<std::vector<pg::Quadtree<entt::entity>::Result>>();
    if (!res.empty())
    {
        // pick an index
        auto index = pg::randomBetween(static_cast<size_t>(0), res.size() - 1);
        drone.targetPos = res[index].box.midpoint();
        drone.hasTarget = true;
        drone.targetId = res[index].data.front();
        auto&& [starsystem, sys_faction] =
            game.getRegistry().get<galaxy::StarSystemState, galaxy::Faction>(drone.targetId);
        sys_faction = faction;
        starsystem.colonizationStatus = galaxy::ColonizationStatus::Planned;
        return false;
    }
    else
    {
        // kill the drone
        // std::cout << "No target found in range\n";
        game.getRegistry().destroy(entity);

        return false;
    }

    return true;
}
