#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgEngine/core/Gui.hpp>
#include <memory>
#include <gui/SplashScreenWidget.hpp>
#include <sndX/BufferCache.hpp>

#include <atomic>

namespace galaxy {

class PreLoadResources : public pg::game::Scene
{
public:
    using pg::game::Scene::Scene;

    virtual ~PreLoadResources() = default;

    void start() override
    {
        // add percentage of resources loaded as singleton
        addSingleton_as<float&>("resourceLoader.totalProgress", _percentTotalResourcesLoaded);
        addSingleton_as<float&>("resourceLoader.currentProgress", _percentCurrentResourceLoaded);
        std::vector<std::string> files = {
            "../data/music/a-meditation-through-time-amp-space-11947.mp3",
            "../data/music/dead-space-style-ambient-music-184793.mp3",
            "../data/music/universe-cosmic-space-ambient-interstellar-soundscape-sci-fi-181916.mp3"

        };
        //
        // start loading thread
        auto size = files.size();
        // start a watcher thread to update the progress
        std::jthread([&, this, size] {
            while (_numRead < size)
            {
                std::unique_lock<std::mutex> lk(_mutex);
                _cv.wait(lk);

                _percentTotalResourcesLoaded = static_cast<float>(_numRead) / size;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }).detach();

        for (auto& file : files)
        {
            std::jthread([&, this, file] {
                // TODO callback per file
                try
                {
                    _cache.get(file);
                }
                catch (const std::exception&)
                {
                    // log
                }
                _percentCurrentResourceLoaded = 1.0;
                _numRead++;
                std::unique_lock<std::mutex> lk(_mutex);
                _cv.notify_one();
            }).detach();
        }

        setupOverlay();
        // load resources for coming scene(s)

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
            {std::make_unique<galaxy::gui::LoadResourcesWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});
    }

private:
    float _percentTotalResourcesLoaded{};

    float _percentCurrentResourceLoaded{};

    std::atomic<uint32_t>     _numRead{};
    soundEngineX::BufferCache _cache;
    // mutex and condition variable to signal when a resource is loaded

    std::mutex              _mutex;
    std::condition_variable _cv;
};
} // namespace galaxy