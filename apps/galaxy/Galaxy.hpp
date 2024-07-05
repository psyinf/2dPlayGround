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
#include "components/StarSystem.hpp"
#include "components/Faction.hpp"
#include "pgGame/components/RenderState.hpp"
#include "events/PickEvent.hpp"
#include <pgGame/components/WindowDetails.hpp>
#include <Config.hpp>
#include <cmath>
#include <ranges>

#include <systems/StatsSystem.hpp>

namespace galaxy {

class GalacticCore

{
public:
    GalacticCore();

    void setup();

    void run();

private:
    std::unique_ptr<pg::game::Game> game;
};

} // namespace galaxy