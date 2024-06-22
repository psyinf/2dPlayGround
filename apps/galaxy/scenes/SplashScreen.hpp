#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgEngine/core/Gui.hpp>
#include <memory>
#include <gui/SplashScreenWidget.hpp>

namespace galaxy {

class SplashScreen : public pg::game::Scene
{
public:
    using pg::game::Scene::Scene;

    void start() override
    {
        auto& systems = getSystems();
        auto& game = getGame();
        systems.emplace_back(std::make_unique<galaxy::RenderSystem>(game));

        setupOverlay();

        Scene::start();
    };

    void setupOverlay()
    {
        // update events

        pg::game::makeEntity<pg::game::GuiDrawable>(getGame().getRegistry(),
                                                    {std::make_unique<pg::game::GuiBegin>(), pg::game::DRAWABLE_FIRST});

        pg::game::makeEntity<pg::game::GuiDrawable>(getRegistry(),
                                                    {std::make_unique<pg::game::GuiEnd>(), pg::game::DRAWABLE_LAST});

        pg::game::makeEntity<pg::game::GuiDrawable>(
            getRegistry(),
            {std::make_unique<galaxy::gui::SplashScreenWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});
    }

private:
};
} // namespace galaxy