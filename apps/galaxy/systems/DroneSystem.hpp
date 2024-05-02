#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>
#include <Config.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;
struct Drone;
struct Faction;

class DroneSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup();

    void makeDrone(pg::fVec2 pos, galaxy::Faction);

    void handle(const pg::game::FrameStamp& frameStamp);

private:
    void createFactions(const pg::game::FrameStamp& frameStamp);
    /**
     * @brief check if there is a terminal failure
     * @return false if handling can be terminated for this entity
     */
    bool checkForFailure(galaxy::Drone& drone, entt::entity entity);

    bool handleProduction(entt::entity entity);

    bool findNewTarget(entt::entity entity);
};
} // namespace galaxy