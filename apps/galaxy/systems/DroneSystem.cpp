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
        game.getRegistry(), {.pos{pos}, .scale{0.005, 0.005}}, pg::game::Drawable{dot_sprite}, {});
}

void galaxy::DroneSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    auto& quadtree = game.getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
    auto  view = game.getRegistry().view<galaxy::Drone, pg::Transform2D>();
    for (auto entity : view)
    {
        auto&& [drone, transform] = view.get<galaxy::Drone, pg::Transform2D>(entity);
        drone.lifetime += 1;
        // exponential failure rate
        auto l = 1.0 - (std::exp(0.000001f * drone.lifetime) - 1);

        if (pg::randomBetween(0.0f, 1.0f) > l)
        {
            game.getRegistry().destroy(entity);
            std::cout << "Died at " << drone.lifetime << " with likelihood: " << l << "\n";
            continue;
        }

        // find nearest star
        if (lengthSquared(transform.pos - drone.targetPos) < 0.01f)
        {
            transform.pos = drone.targetPos;
            drone.hasTarget = false;
            drone.atTarget = true;
            drone.waitCycles = 100;
            // mark the star as visited
            auto& starsystem = game.getRegistry().get<galaxy::StarSystemState>(drone.targetId);
            if (starsystem.colonizationStatus == galaxy::ColonizationStatus::Explored)
            {
                // suicide
                game.getRegistry().destroy(entity);
                continue;
            }

            starsystem.colonizationStatus = galaxy::ColonizationStatus::Explored;
        }
        if (drone.atTarget)
        {
            drone.waitCycles -= 1;
            if (drone.waitCycles == 0)
            {
                drone.atTarget = false;
                drone.hasTarget = false;
                drone.targetId = entt::null;
                // build new drones
                // TODO: constant for replication factor
                makeDrone(transform.pos);
                makeDrone(transform.pos);
            }
        }
        if (!drone.hasTarget)
        { // filter out own position using ranges
            auto r =

                quadtree.rangeQuery(pg::fBox::fromMidpoint(transform.pos, {40, 40})) |
                std::views::filter([&transform](auto result) { return !equal(result.box.midpoint(), transform.pos); }) |
                // remove stars that are already visited
                std::views::filter([&game = game](auto result) {
                    auto& starsystem = game.getRegistry().get<galaxy::StarSystemState>(result.data.front());
                    return starsystem.colonizationStatus == galaxy::ColonizationStatus::Unexplored;
                })

                ;
            // to vector
            auto res = std::ranges::to<std::vector<pg::Quadtree<entt::entity>::Result>>(r);
            if (!r.empty())
            {
                // pick an index
                auto index = pg::randomBetween(static_cast<size_t>(0), res.size() - 1);
                drone.targetPos = res[index].box.midpoint();
                drone.hasTarget = true;
                drone.targetId = res[index].data.front();
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
