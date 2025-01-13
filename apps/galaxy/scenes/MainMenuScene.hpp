#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgEngine/core/Gui.hpp>
#include <memory>
#include <gui/MainMenuWidget.hpp>
#include <components/SoundScape.hpp>
#include <pgGame/core/ResourcePreLoader.hpp>

namespace galaxy {

class MainMenuScene : public pg::game::Scene
{
public:
    using pg::game::Scene::Scene;

    virtual ~MainMenuScene() = default;

    void start() override
    {
        if (started()) { return; }
        auto& loaders = getGame().getSingleton<pg::singleton::RegisteredLoaders>(getId() + ".loaders");
        _resourcePreloader.initialize(loaders.loaders, _filesLoaded, [this](float p) { _totalLoaded = p; });

        setupOverlay();
        Scene::start();
    };

    void setupOverlay()
    {
        // update events
        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiBegin>(), pg::game::DRAWABLE_FIRST});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiEnd>(), pg::game::DRAWABLE_LAST});

        pg::game::makeEntity<pg::game::GuiDrawable>(
            getSceneRegistry(),
            {std::make_unique<galaxy::gui::MainMenuWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});
    }

private:
    ResourcePreloader   _resourcePreloader;
    PercentCompletedMap _filesLoaded;
    float               _totalLoaded{0.0f};
};
} // namespace galaxy