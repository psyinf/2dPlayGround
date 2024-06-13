#pragma once
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/components/Drawable.hpp>
#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/primitives/GuiRenderable.hpp>
#include <pgEngine/math/VecUtils.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <components/Tags.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/UpdateSystem.hpp>
#include <systems/PickingSystem.hpp>
#include <systems/DroneSystem.hpp>
#include <systems/LifetimeSystem.hpp>
#include <systems/BehaviorSystem.hpp>
#include <systems/LambdaSystem.hpp>
#include "components/StarSystem.hpp"
#include "components/Faction.hpp"
#include "pgGame/components/RenderState.hpp"
#include "events/PickEvent.hpp"
#include <pgGame/components/WindowDetails.hpp>
#include <Config.hpp>
#include <cmath>
#include <ranges>
#include <gui/SystemInfo.hpp>
#include <gui/DashBoardWidget.hpp>
#include <systems/StatsSystem.hpp>
#include <gui/StatsWidget.hpp>

namespace galaxy {

template <typename Func>
auto makeLambdaSystem(pg::game::Game& game, Func&& f)
{
    return std::make_unique<LambdaSystem<Func>>(game, std::forward<Func>(f));
}

class GalacticCore

{
public:
    GalacticCore(config::Galaxy&& config);

    void setup();

    void run();

private:
    void setupConfig();

    void setupSelectionMarker();

    void setupQuadtreeDebug();

    void setupOverlay();

    void setupGalaxy();

private:
    std::unique_ptr<pg::game::Game>             game;
    std::unique_ptr<pg::Gui>                    gui;
    std::unique_ptr<pg::Quadtree<entt::entity>> galaxyQuadtree;
    bool                                        isDragging{};
    bool                                        drawDebugItems{true};
    bool                                        drawQuadTree{false};
    config::Galaxy                              galaxyConfig;
};

} // namespace galaxy