#include "DroneSystem.hpp"
#include <entities/Drone.hpp>
#include <entities/StarSystem.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/VecOperations.hpp>

void galaxy::DroneSystem::setup()
{
    auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>();
    auto entity = view.front();
    auto&& [drawable, transform, starsystem] =
        view.get<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>(entity);
    // get a single star system

    makeDrone(transform.pos);
}

void galaxy::DroneSystem::makeDrone(pg::fVec2 pos)
{
    auto dot_sprite = game.getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");
    // create a drone
    pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::Drone>(
        game.getRegistry(), {.pos{pos}, .scale{0.01, 0.01}}, pg::game::Drawable{dot_sprite}, {});
}

void galaxy::DroneSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    auto& quadtree = game.getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
    auto  view = game.getRegistry().view<galaxy::Drone, pg::Transform2D>();
    for (auto entity : view)
    {
        auto&& [drone, transform] = view.get<galaxy::Drone, pg::Transform2D>(entity);
        // find nearest star
        if (lengthSquared(transform.pos - drone.targetPos) < 0.01f)
        {
            transform.pos = drone.targetPos;
            drone.hasTarget = false;
            drone.atTarget = true;
            drone.waitCycles = 100;
            // mark the star as visited
        }
        if (drone.atTarget)
        {
            drone.waitCycles -= 1;
            if (drone.waitCycles == 0)
            {
                drone.atTarget = false;
                drone.hasTarget = false;
                // build new drones
                makeDrone(transform.pos);
            }
        }
        if (!drone.hasTarget)
        { // filter out own position using ranges
            auto r =

                quadtree.rangeQuery(pg::fBox::fromMidpoint(transform.pos, {20, 20})) |
                std::views::filter([&transform](auto result) { return !equal(result.box.midpoint(), transform.pos); });
            // to vector
            auto res = std::ranges::to<std::vector<pg::Quadtree<entt::entity>::Result>>(r);
            if (!r.empty())
            {
                // pick an index
                auto index = pg::randomBetween(static_cast<size_t>(0), res.size() - 1);
                drone.targetPos = res[index].box.midpoint();
                drone.hasTarget = true;
            }
            else
            {
                // kill the drone
                game.getRegistry().destroy(entity);
            }
        }
        // start moving towards the target, simulate some acceleration
        transform.pos += (drone.targetPos - transform.pos) * 0.01f;
    }
}
