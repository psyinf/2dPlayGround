#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <memory>

#include <pgEngine/core/LoadSave.hpp>
#include <pgEngine/resources/SpriteResource.hpp>

#include <gui/StatsWidget.hpp>
#include <gui/SystemInfo.hpp>
#include <gui/DashBoardWidget.hpp>

#include <gui/StarSystemWidget.hpp>
#include <gui/LoadResourcesWidget.hpp>

#include <serializer/ConfigSerializer.hpp>
#include <systems/GuiRenderSystem.hpp>

namespace galaxy {

class SystemScene : public pg::game::Scene
{
public:
    SystemScene(pg::game::Game& game, pg::game::SceneConfig&& cfg)
      : pg::game::Scene(game, std::move(cfg))
    {
    }

    virtual ~SystemScene() = default;

    void start() override
    {
        auto& systems = getSystems();
        auto& game = getGame();

        setupOverlay();

        Scene::start();
    }

    void stop() override { Scene::stop(); }

private:
    void setupOverlay()
    {
        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiBegin>(), pg::game::DRAWABLE_FIRST});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiEnd>(), pg::game::DRAWABLE_LAST});

        pg::game::makeEntity<pg::game::GuiDrawable>(
            getSceneRegistry(),
            {std::make_unique<galaxy::gui::StarSystemWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});
    }

private:
    std::unique_ptr<pg::Quadtree<entt::entity>> galaxyQuadtree;
    config::Galaxy                              galaxyConfig;
    bool                                        isDragging{};
};

} // namespace galaxy