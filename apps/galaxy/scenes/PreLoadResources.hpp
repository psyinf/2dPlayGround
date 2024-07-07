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
    static constexpr bool coarse = false;

public:
    using pg::game::Scene::Scene;

    virtual ~PreLoadResources() = default;

    void start() override
    {
        // add percentage of resources loaded as singleton
        addSingleton_as<float&>("resourceLoader.totalProgress", _percentTotalResourcesLoaded);
        addSingleton_as<float&>("resourceLoader.currentProgress", _percentCurrentResourceLoaded);
        addSingleton_as<std::map<std::string, float>&>("resourceLoader.resourcesProgress", _percentResourcesLoaded);
        // TODO: from configuration
        // TODO: resource-cache abstraction (e.g. sound::background1 -> filename)
        std::vector<std::string> resources = {"sound::background1", "sound::background2", "sound::background3"};
        // use resource cache to load resources

        std::vector<std::string> files = {
            "../data/music/a-meditation-through-time-amp-space-11947.mp3",
            "../data/music/dead-space-style-ambient-music-184793.mp3",
            "../data/music/universe-cosmic-space-ambient-interstellar-soundscape-sci-fi-181916.mp3"};

        // start loading thread
        auto size = files.size();
        // start a watcher thread to update the progress
        createWatchProgressThread(size);

        for (auto& file : files)
        {
            createLoaderThread(file);
        }

        setupOverlay();
        // load resources for coming scene(s)

        Scene::start();
    };

    void createLoaderThread(const std::string& file)
    {
        std::jthread([this, file] {
            try
            {
                _cache.get(file, {file, [&](soundEngineX::loader::LoadProgressInfo progress) {
                                      _percentCurrentResourceLoaded = progress.percent();
                                      _percentResourcesLoaded[file] = progress.percent();
                                  }});
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            catch (const std::exception&)
            {
                // log
                spdlog::error("Failed to load resource: {}", file);
            }
            _percentResourcesLoaded[file] =
                1.0f; // might have been in cache already, thus not triggering a progress callback
            _percentCurrentResourceLoaded = 1.0f;
            _numRead++;
            std::unique_lock<std::mutex> lk(_mutex);
            _cv.notify_one();
        }).detach();
    }

    void createWatchProgressThread(std::vector<std::string>::size_type size)
    {
        if (coarse)
        {
            std::jthread([&, this, size] {
                while (_numRead < size)
                {
                    std::unique_lock<std::mutex> lk(_mutex);
                    _cv.wait(lk);

                    _percentTotalResourcesLoaded = static_cast<float>(_numRead) / size;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }).detach();
        }
        else
        {
            std::jthread([&, this, size] {
                while (_numRead < size)
                {
                    using namespace std::chrono_literals;
                    std::unique_lock<std::mutex> lk(_mutex);
                    _cv.wait_for(lk, 50ms);

                    _percentTotalResourcesLoaded = 0.0;
                    for (const auto& [key, value] : _percentResourcesLoaded)
                    {
                        _percentTotalResourcesLoaded += (value / static_cast<float>(size));
                    }
                    //_percentTotalResourcesLoaded /= size;
                }
            }).detach();
        }
    }

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

    std::map<std::string, float> _percentResourcesLoaded{};

    std::atomic<uint32_t>     _numRead{};
    soundEngineX::BufferCache _cache;
    // mutex and condition variable to signal when a resource is loaded

    std::mutex              _mutex;
    std::condition_variable _cv;
};
} // namespace galaxy