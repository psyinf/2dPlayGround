#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgEngine/core/Gui.hpp>
#include <memory>
#include <gui/SplashScreenWidget.hpp>
#include <sndX/SoundEngine.hpp>
#include <sndX/BackgroundPlayer.hpp>

namespace galaxy {

class SplashScreen : public pg::game::Scene
{
public:
    SplashScreen(pg::game::Game& game)
      : pg::game::Scene(game)
      , soundEngine(std::make_unique<soundEngineX::SoundEngine>())
      , backgroundPlayer(std::make_unique<soundEngineX::BackgroundPlayer>())
    {
        // TODO: fix paths
        try
        {
            backgroundPlayer->load("../data/music/dead-space-style-ambient-music-184793.mp3");
        }
        catch (const std::exception& e)
        {
            spdlog::error("Failed to load resources {}", e.what());
        }
    }

    void start() override
    {
        try
        {
            // TODO: resources repo should be configurable regarding its target path
            backgroundPlayer->play("../data/music/dead-space-style-ambient-music-184793.mp3");
        }
        catch (const std::exception& e)
        {
            spdlog::error("Failed to play sound {}", e.what());
        }

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

    void stop() override
    {
        soundEngine->stopAll();
        backgroundPlayer->forceCheckPending();
        backgroundPlayer = nullptr;
        Scene::stop();
    }

private:
    std::unique_ptr<soundEngineX::SoundEngine>      soundEngine;
    std::unique_ptr<soundEngineX::BackgroundPlayer> backgroundPlayer;
};
} // namespace galaxy