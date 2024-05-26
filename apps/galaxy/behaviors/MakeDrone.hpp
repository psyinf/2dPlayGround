#pragma once

#include <behaviortree_cpp/action_node.h>
#include <pgEngine/math/Transform.hpp>
#include <components/Drone.hpp>
#include <pgEngine/math/Vec.hpp>

namespace behavior {

class MakeDrone : public BehaviorActionNode
{
    using BehaviorActionNode::BehaviorActionNode;

public:
    static BT::PortsList providedPorts() { return {}; }

    // You must override the virtual function tick()
    BT::NodeStatus onStart() override
    {
        auto& game = this->game();
        auto  maker_entity = config().blackboard->get<entt::entity>("entity");
        auto  view = game.getRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Faction>();
        auto&& [drone, transform, faction] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Faction>(maker_entity);

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
                                                        {.pos{transform.pos}, .scale{0.00125, 0.00125}},
                                                        pg::game::Drawable{dot_sprite},
                                                        {galaxy::Drone::fromConfig(drone_params)},
                                                        galaxy::Dynamic{},
                                                        std::move(faction),
                                                        galaxy::Lifetime{},
                                                        {std::move(renderState)});

        // add behavior

        auto behavior_tree = ctx().factory.createTreeFromFile("../data/behaviors/drones.xml");

        auto visitor = [&entity](BT::TreeNode* node) {
            if (auto action_B_node = dynamic_cast<behavior::BehaviorActionNode*>(node))
            {
                action_B_node->setEntity(entity);
            }
        };

        // Apply the visitor to ALL the nodes of the tree
        behavior_tree.applyVisitor(visitor);
        behavior_tree.rootBlackboard()->set("entity", entity);
        pg::game::addComponent<galaxy::Behavior>(
            game.getRegistry(), entity, galaxy::Behavior{std::move(behavior_tree)});

        game.getDispatcher().trigger<galaxy::events::DroneCreatedEvent>({entity, transform.pos});
    }
};

} // namespace behavior