#include "DroneSystem.hpp"
#include <components/Drone.hpp>
#include <components/Lifetime.hpp>
#include <components/StarSystem.hpp>

#include <pgGame/components/Drawable.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/Vec.hpp>
#include <Config.hpp>
#include <components/Faction.hpp>

#include <components/Behavior.hpp>
#include <pgEngine/math/Random.hpp>
#include <events/DroneEvents.hpp>

#include <ranges>
#include <pgGame/components/RenderState.hpp>
#include <behaviors/FindNextSystem.hpp>
#include <behaviors/Travel.hpp>
#include <behaviors/MakeDrone.hpp>
#include <behaviors/GetTargetsAvailable.hpp>

#include <behaviors/EntityQueueLoop.hpp>

void galaxy::DroneSystem::setup(std::string_view /*scene_id*/)
{
    ctx = std::make_shared<behavior::Context>(&_game, std::make_unique<BT::BehaviorTreeFactory>());
    _game.getDispatcher().sink<galaxy::events::DroneFailedEvent>().connect<&galaxy::DroneSystem::handleDroneFailed>(
        *this);
    auto& factory = ctx->factory();
    factory.registerSimpleCondition("CheckForDamage", [&](BT::TreeNode&) { return BT::NodeStatus::SUCCESS; });
    factory.registerNodeType<behavior::FindNextSystem>("FindNextSystem", ctx);
    factory.registerNodeType<behavior::Travel>("Travel", ctx);
    factory.registerNodeType<behavior::GetTargetsAvailable>("GetTargetsAvailable", ctx);
    factory.registerNodeType<behavior::MakeDrone>("MakeDrone", ctx);
    // factory.registerNodeType<BT::LoopNode<entt::entity>>("LoopProductionQueue");
    factory.registerNodeType<behavior::EntityQueueLoop>("LoopProductionQueue", ctx);
    factory.registerSimpleAction("Terminate", [this](const BT::TreeNode& node) {
        // downcast to access the entity
        auto entity = node.config().blackboard->get<entt::entity>("entity");
        spdlog::debug("Drone {} failed", entt::to_integral(entity));
        _game.getDispatcher().enqueue<galaxy::events::DroneFailedEvent>({.entity = entity});
        return BT::NodeStatus::SUCCESS;
    });
    factory.registerBehaviorTreeFromFile("../data/behaviors/drones.xml");
}

void galaxy::DroneSystem::handle(const pg::FrameStamp& frameStamp)
{
    createFactions(frameStamp);
}

void galaxy::DroneSystem::handleDroneFailed(galaxy::events::DroneFailedEvent event)
{
    // later: decide if the drone dies or becomes a drifter
    // for now send a signal to destroy the entity
    auto& drone = _game.getGlobalRegistry().get<galaxy::Drone>(event.entity);
    if (drone.targetId != entt::null)
    {
        auto&& [starsystem, faction] =
            _game.getGlobalRegistry().get<galaxy::StarSystemState, galaxy::Faction>(drone.targetId);
        // TODO: unmark only if planned by our faction
        if (starsystem.colonizationStatus == galaxy::ColonizationStatus::Planned)
        {
            starsystem.colonizationStatus = galaxy::ColonizationStatus::Unexplored;
        }
    }
    _game.getGlobalRegistry().destroy(event.entity);
}

void galaxy::DroneSystem::createFactions(const pg::FrameStamp& frameStamp)
{
    static std::random_device rd;
    static std::mt19937       gen{rd()};
    // setup for all factions
    auto galaxy_config = _game.getCurrentScene().getSingleton<const galaxy::config::Galaxy&>("galaxy.config");

    for (const auto& faction : galaxy_config.factions)
    {
        // TODO: this needs to be based on the real time passed (e.g. years)
        if (faction.startParams.start_cycle != frameStamp.gameTick) { continue; }

        auto view = _game.getGlobalRegistry()
                        .view<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState, galaxy::Faction>();
        auto it = view.begin();
        auto size = static_cast<size_t>(std::distance(view.begin(), view.end()));

        if (size == 0)
        {
            spdlog::error("No star systems found");
            continue;
        }

        std::advance(it, pg::randomBetween<size_t>(0, size));
        auto entity = *it;
        auto&& [drawable, transform, starsystem, system_faction] =
            view.get<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState, galaxy::Faction>(entity);
        // get a single star system
        system_faction = {faction.name, faction.color, faction.color};
        starsystem.colonizationStatus = galaxy::ColonizationStatus::Colonized;
        // add faction
        spdlog::info("Create faction {}", system_faction.name);

        // setup port connections
        // defaults

        ctx->injectors["GetTargetsAvailable"].input("max_targets_to_find", "{max_targets_to_find}");
        ctx->injectors["GetTargetsAvailable"].output("available_target_list", "{available_target_list}");
        ctx->injectors["LoopProductionQueue"].input("queue", "{available_target_list}", true);

        BT::Blackboard::Ptr blackboard = BT::Blackboard::create();
        blackboard->set("max_targets_to_find", faction.startParams.num_start_drones);
        blackboard->set("ID", fmt::format("Seed: {}", entt::to_integral(entity)));
        blackboard->set("entity", entt::to_integral(entity));
        auto behavior_tree = ctx->setupTree("Seed", entity, blackboard);

        pg::game::addComponent<galaxy::Behavior>(
            _game.getGlobalRegistry(), entity, galaxy::Behavior{std::move(behavior_tree)});
    }
}
