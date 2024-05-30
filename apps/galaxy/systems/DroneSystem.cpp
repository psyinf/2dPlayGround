#include "DroneSystem.hpp"
#include <components/Drone.hpp>
#include <components/Lifetime.hpp>
#include <components/StarSystem.hpp>

#include <pgGame/components/Drawable.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/VecOperations.hpp>
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

#include <behaviors/TestLoop.hpp>

void galaxy::DroneSystem::setup()
{
    ctx = std::make_shared<behavior::Context>(&game);
    game.getDispatcher().sink<galaxy::events::DroneFailedEvent>().connect<&galaxy::DroneSystem::handleDroneFailed>(
        *this);
    auto& factory = ctx->factory();
    factory.registerSimpleCondition("CheckForDamage", [&](BT::TreeNode& node) { return BT::NodeStatus::SUCCESS; });
    factory.registerNodeType<behavior::FindNextSystem>("FindNextSystem", ctx);
    factory.registerNodeType<behavior::Travel>("Travel", ctx);
    factory.registerNodeType<behavior::GetTargetsAvailable>("GetTargetsAvailable", ctx);
    factory.registerNodeType<behavior::MakeDrone>("MakeDrone", ctx);
    // factory.registerNodeType<BT::LoopNode<entt::entity>>("LoopProductionQueue");
    factory.registerNodeType<behavior::TestLoop>("LoopProductionQueue");
    factory.registerSimpleAction("Terminate", [this](const BT::TreeNode& node) {
        // auto entity = node.getInput<entt::entity>("entity");
        // game.getDispatcher().enqueue<galaxy::events::DroneFailedEvent>({entity.value()});
        return BT::NodeStatus::SUCCESS;
    });
    factory.registerBehaviorTreeFromFile("../data/behaviors/drones.xml");
}

void galaxy::DroneSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    createFactions(frameStamp);
}

void galaxy::DroneSystem::handleDroneFailed(galaxy::events::DroneFailedEvent& event)
{
    // later: decide if the drone dies or becomes a drifter
    // for now send a signal to destroy the entity
    auto& drone = game.getRegistry().get<galaxy::Drone>(event.entity);
    if (drone.targetId != entt::null)
    {
        auto&& [starsystem, faction] = game.getRegistry().get<galaxy::StarSystemState, galaxy::Faction>(drone.targetId);
        // TODO: unmark only if planned by our faction
        if (starsystem.colonizationStatus == galaxy::ColonizationStatus::Planned)
        {
            starsystem.colonizationStatus = galaxy::ColonizationStatus::Unexplored;
        }
    }
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

        auto view =
            game.getRegistry().view<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState, galaxy::Faction>();
        auto it = view.begin();
        auto size = static_cast<size_t>(std::distance(view.begin(), view.end()));
        std::advance(it, pg::randomBetween<size_t>(0, size));
        auto entity = *it;
        auto&& [drawable, transform, starsystem, system_faction] =
            view.get<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState, galaxy::Faction>(entity);
        // get a single star system
        system_faction = {faction.name, faction.color, faction.color};
        starsystem.colonizationStatus = galaxy::ColonizationStatus::Colonized;
        // add faction

        std::cout << "createFactions " << entt::to_integral(entity) << " " << system_faction.name << "\n";
        // add seed behaviortree
        ctx->injectors["GetTargetsAvailable"].input("max_targets_to_find", "{max_targets_to_find}");
        auto behavior_tree =
            ctx->setupTree("Seed", entity);
        pg::game::addComponent<galaxy::Behavior>(
            game.getRegistry(), entity, galaxy::Behavior{std::move(behavior_tree)});
    }
}
