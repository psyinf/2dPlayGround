#pragma once

#include <pgEngine/math/Transform.hpp>
#include <components/Drone.hpp>
#include <behaviors/utils/BehaviorActionNode.hpp>
#include <helpers/GalaxyHelpers.hpp>
#include <components/Tags.hpp>
#include <components/Lifetime.hpp>
#include <pgGame/components/RenderState.hpp>

namespace behavior {

class MakeDrone : public BehaviorActionNode
{
    using BehaviorActionNode::BehaviorActionNode;

public:
    static BT::PortsList providedPorts()
    {
        return {BT::OutputPort<std::vector<entt::entity>>("available_target_list"), //
                BT::InputPort<std::vector<entt::entity>>("available_target_list")};
    }

    // You must override the virtual function tick()
    BT::NodeStatus onStart() override
    {
        auto& game = this->game();
        auto  maker_entity = entity();
        auto  view = game.getGlobalRegistry().view<pg::Transform2D, galaxy::Faction>();

        auto&& [transform, faction] = view.get<pg::Transform2D, galaxy::Faction>(maker_entity);

        auto        dot_sprite = game.getResource<pg::Sprite>("::resources/circle_05.png");
        const auto& drone_params = galaxy::getFactionConfig(game, faction).droneParams;
        const auto& faction_start_params = galaxy::getFactionConfig(game, faction).startParams;
        // create a drone
        auto renderState = pg::States{};
        renderState.push(pg::TextureColorState{faction.entityColor});
        auto f = faction;
        auto entity = //
            pg::game::makeEntity<pg::Transform2D,
                                 pg::game::Drawable,
                                 galaxy::Drone,
                                 galaxy::Dynamic,
                                 galaxy::Faction,
                                 galaxy::Lifetime,
                                 pg::game::RenderState,
                                 pg::tags::GalaxyRenderTag>(
                game.getGlobalRegistry(),
                {.pos{transform.pos}, .scale{0.00125f, 0.00125f}},
                pg::game::Drawable{dot_sprite},
                {galaxy::Drone::fromConfig(drone_params)},
                galaxy::Dynamic{},
                std::move(f),
                galaxy::Lifetime{.expected_lifetime{drone_params.expected_lifetime},
                                 .reliability_factor{drone_params.reliability_factor}},
                {std::move(renderState)},
                {});

        // add behavior"

        // setup port connections
        // initial queue size is setup

        auto blackboard = BT::Blackboard::create();
        blackboard->set("max_targets_to_find", faction_start_params.num_start_drones);
        blackboard->set("ID", std::format("Drone: {}", entt::to_integral(entity)));
        blackboard->set("entity", entt::to_integral(entity));

        auto behavior_tree = ctx()->setupTree("Drone", entity, blackboard);

        pg::game::addComponent<galaxy::Behavior>(
            game.getGlobalRegistry(), entity, galaxy::Behavior{std::move(behavior_tree)});

        game.getDispatcher().trigger<galaxy::events::DroneCreatedEvent>({.entity = entity, .transform = transform.pos});
        return BT::NodeStatus::SUCCESS;
    }

    BT::NodeStatus onRunning() override
    {
        // should not be called
        return BT::NodeStatus::SUCCESS;
    }
};
} // namespace behavior