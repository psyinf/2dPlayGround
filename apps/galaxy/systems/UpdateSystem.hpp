#pragma once
#include <entt/entt.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <entities/Faction.hpp>
#include <entities/StarSystem.hpp>

#include <events/DroneEvents.hpp>
#include <pgGame/entities/RenderState.hpp>

#include <Config.hpp>
#include <iostream>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class UpdateSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup() override
    {
        game.getDispatcher().sink<galaxy::events::DroneCreatedEvent>().connect<&UpdateSystem::handleDroneCreated>(this);
    };

    void handleDroneCreated(galaxy::events::DroneCreatedEvent& event)
    {
        // TODO: use an animation

        auto& transform = game.getRegistry().get<pg::Transform2D>(event.entity);
        auto& marker_transform =
            game.getRegistry().get<pg::Transform2D>(game.getSingleton<entt::entity>("galaxy.debug.marker"));
        marker_transform.pos = transform.pos;
    }

    void handle(const pg::game::FrameStamp& frameStamp) override
    {
        auto& fpsCounter = game.getApp().getFPSCounter();
        if (fpsCounter.getCurrentFrameCount() % 100 == 0) { std::cout << fpsCounter.getAverageFPSAndReset() << "\n"; }

        updateStarSystems(frameStamp);
        // move the stars on a circle at the distance from center
        //             auto distance = normalize(transform.pos);
        //             auto angle = atan2(transform.pos[1], transform.pos[0]);
        //             angle += 0.0001 * sqrt(1.0 / distance);
        //             transform.pos = {distance * cos(angle), distance * sin(angle)};
        //
    }

    void updateStarSystems(const pg::game::FrameStamp& frameStamp)
    {
        // get config from singleton
        auto& galaxyConfig = game.getSingleton<const galaxy::config::Galaxy&>("galaxy.config");

        auto& registry = game.getRegistry();
        auto  view = registry.view<pg::game::Drawable,
                                  pg::Transform2D,
                                  galaxy::StarSystemState,
                                  pg::game::RenderState,
                                  galaxy::Faction>();
        for (auto& entity : view)
        {
            auto&& [drawable, transform, starSystemState, state, faction] = view.get<pg::game::Drawable,
                                                                                     pg::Transform2D,
                                                                                     galaxy::StarSystemState,
                                                                                     pg::game::RenderState,
                                                                                     galaxy::Faction>(entity);
            state.states = std::move(pg::States{});
            switch (starSystemState.colonizationStatus)
            {
            case galaxy::ColonizationStatus::Explored:

                state.states.push(pg::TextureColorState{faction.entityColor});
                break;
            case galaxy::ColonizationStatus::Planned:
                state.states.push(pg::TextureColorState{scale(faction.entityColor, 0.2)});
                break;
            default:
                state.states.push(pg::TextureColorState{galaxyConfig.star.default_color});
                break;
            }
        }
    }
};
} // namespace galaxy