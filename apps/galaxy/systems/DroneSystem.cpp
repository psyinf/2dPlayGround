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
#include <behaviors/FindNextTarget.hpp>
#include <behaviors/Travel.hpp>
#include <behaviors/Produce.hpp>

#include <behaviors/MakeDrone.hpp>
#include <behaviors/GetTargetsAvailable.hpp>

void galaxy::DroneSystem::setup()
{
    game.getDispatcher().sink<galaxy::events::DroneFailedEvent>().connect<&galaxy::DroneSystem::handleDroneFailed>(
        *this);
    auto ctx = std::make_shared<behavior::Context>(game, factory);
    factory.registerSimpleCondition("CheckForDamage", [&](BT::TreeNode& node) { return BT::NodeStatus::SUCCESS; });
    factory.registerNodeType<behavior::FindNextSystem>("FindNextSystem", ctx);
    factory.registerNodeType<behavior::Travel>("Travel", ctx);

    factory.registerSimpleAction("BuildDrone", [this](const BT::TreeNode& node) {
        auto entity = node.config().blackboard->get<entt::entity>("entity");
        auto view = game.getRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Faction>();
        auto&& [drone, transform, faction] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Faction>(entity);

        makeDrone(transform.pos, faction);
        return BT::NodeStatus::SUCCESS;
    });

    factory.registerSimpleAction("Terminate", [this](const BT::TreeNode& node) {
        auto entity = node.config().blackboard->get<entt::entity>("entity");
        game.getDispatcher().enqueue<galaxy::events::DroneFailedEvent>({entity});
        return BT::NodeStatus::SUCCESS;
    });
    behavior::GetTargetsAvailable x("", {}, ctx);
    factory.registerNodeType<behavior::GetTargetsAvailable, std::shared_ptr<behavior::Context>>("GetTargetsAvailable",
                                                                                                ctx);
    //  factory.registerNodeType<behavior::BuildDrone>("BuildDrone", &game);
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

    auto entity = //
        pg::game::makeEntity<pg::Transform2D,
                             pg::game::Drawable,
                             galaxy::Drone,
                             galaxy::Dynamic,
                             galaxy::Faction,
                             galaxy::Lifetime,
                             pg::game::RenderState>(game.getRegistry(),
                                                    {.pos{pos}, .scale{0.00125, 0.00125}},
                                                    pg::game::Drawable{dot_sprite},
                                                    {Drone::fromConfig(drone_params)},
                                                    galaxy::Dynamic{},
                                                    std::move(faction),
                                                    galaxy::Lifetime{},
                                                    {std::move(renderState)});

    // add behavior
    auto behavior_tree = factory.createTreeFromFile("../data/behaviors/drones.xml");

    auto visitor = [&entity](BT::TreeNode* node) {
        if (auto action_B_node = dynamic_cast<behavior::BehaviorActionNode*>(node))
        {
            action_B_node->setEntity(entity);
        }
    };

    // Apply the visitor to ALL the nodes of the tree
    behavior_tree.applyVisitor(visitor);
    behavior_tree.rootBlackboard()->set("entity", entity);
    pg::game::addComponent<galaxy::Behavior>(game.getRegistry(), entity, galaxy::Behavior{std::move(behavior_tree)});

    game.getDispatcher().trigger<galaxy::events::DroneCreatedEvent>({entity, pos});
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

        auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>();
        auto it = view.begin();
        auto size = static_cast<size_t>(std::distance(view.begin(), view.end()));
        std::advance(it, pg::randomBetween<size_t>(0, size));
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
