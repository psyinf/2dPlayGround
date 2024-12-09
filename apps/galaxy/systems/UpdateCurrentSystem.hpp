#pragma once
#include <entt/entt.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>
#include <pgGame/components/Drawable.hpp>
#include <pgOrbit/OrbitalMechanics.hpp>
#include <components/Faction.hpp>
#include <components/StarSystem.hpp>
#include <components/Orbit.hpp>
#include <pgOrbit/OrbitalParameters.hpp>
#include <pgOrbit/Constants.hpp>

#include <events/DroneEvents.hpp>
#include <pgGame/components/RenderState.hpp>

#include <Config.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class UpdateCurrentSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view /*scene_id*/) override{};

    void handle(const pg::FrameStamp& frameStamp) override { updateSelectedStarSystem(frameStamp); }

    void updateSelectedStarSystem(const pg::FrameStamp& frameStamp)
    {
        auto& registry = _game.getCurrentSceneRegistry();
        auto  view = registry.view<pg::Transform2D, galaxy::Orbiter, pgOrbit::OrbitalParameters<double>>();
        for (auto& entity : view)
        {
            //
            auto&& [transform, orbiter, orbitalParameters] =
                view.get<pg::Transform2D, galaxy::Orbiter, pgOrbit::OrbitalParameters<double>>(entity);
            const auto& test = registry.get<pg::Transform2D>(entity);

            // get passed time
            auto seconds = frameStamp.getFrameDuration_sec();
            //
            //             auto [orbital_parameters, central_body_ref] =
            //                 scene.getRegistry().get<OrbitalParameters, CentralBodyRef>(orbiter.orbit);
            //             auto central_body = scene.getRegistry().get<CentralBody>(central_body_ref.central_body);
            double central_mass = 1.0;
            auto   orbital_period = 2.0 * std::numbers::pi *
                                  std::sqrt(std::pow(orbitalParameters.semimajor_axis, 3.0) /
                                            (pgOrbit::constants::G * (central_mass + orbiter.mass)));
            orbiter.epoch += seconds;
            /*scene_props.calculateEffectiveTimePassedSec(FrameStamp::toSeconds(stamp.frame_time)) / orbital_period;*/
            // TODO: adapt time scales
            auto pos = pgOrbit::OrbitalMechanics<double>::getEulerAngelsAtFraction(
                           orbitalParameters, 5000 * orbiter.epoch / orbital_period)
                           .toCartesian();

            transform.pos = pg::vec_cast<float>(pg::swizzle(pos, pg::XY{}));

            //             auto pos = pg::vec_cast<float>(
            //                 pgOrbit::OrbitalMechanics<double>::getEulerAnglesFromEccentricAnomaly(orbitalParameters,
            //                 orbit, )
            //                     .toCartesian());
        }
    }
};
} // namespace galaxy