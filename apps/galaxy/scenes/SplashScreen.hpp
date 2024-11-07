#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgEngine/core/Gui.hpp>
#include <memory>
#include <gui/SplashScreenWidget.hpp>
#include <components/SoundScape.hpp>
namespace galaxy {

class SplashScreen : public pg::game::Scene
{
public:
    using pg::game::Scene::Scene;

    virtual ~SplashScreen() = default;

    void start() override
    {
        //TODO: from config
        SceneSoundScape soundScape;
        soundScape.background_music = "../data/music/a-meditation-through-time-amp-space-11947.mp3";
        addSingleton_as<SceneSoundScape>("scene.soundScape", soundScape);
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
            {std::make_unique<galaxy::gui::SplashScreenWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});
    }
};
} // namespace galaxy