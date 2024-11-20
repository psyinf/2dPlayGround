#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgEngine/core/Gui.hpp>
#include <memory>
#include <gui/SplashScreenWidget.hpp>
#include <sndX/BufferCache.hpp>

#include <atomic>
#include <barrier>
#include <pgEngine/core/FrameStamp.hpp>

#include <resources/SoundResource.hpp>
#include <pgEngine/generators/MarkovNameGen.hpp>

namespace galaxy {

using Loader = std::function<void()>;

using Loaders = std::map<std::string, Loader>;

class PreLoadResources : public pg::game::Scene
{
    static constexpr bool coarse = false;

public:
    using pg::game::Scene::Scene;

    virtual ~PreLoadResources()
    {
        // wait for threads to finish

        if (_threads_running)
        {
            spdlog::info("Waiting for loading threads to finish");
            _threads_running->arrive_and_wait();
            spdlog::info("Loading threads finished");
        }
    }

    void start() override
    {
        // add percentage of resources loaded as singleton
        addSingleton_as<float&>("resourceLoader.totalProgress", _percentTotalResourcesLoaded);
        addSingleton_as<float&>("resourceLoader.currentProgress", _percentCurrentResourceLoaded);
        addSingleton_as<std::map<std::string, float>&>("resourceLoader.resourcesProgress", _percentResourcesLoaded);
        // TODO: from configuration

        // use resource cache to load resources
        // TODO: collect from config
        std::vector<std::string> sound_files = {
            "../data/music/a-meditation-through-time-amp-space-11947.mp3",
            "../data/music/dead-space-style-ambient-music-184793.mp3",
            "../data/music/universe-cosmic-space-ambient-interstellar-soundscape-sci-fi-181916.mp3"};

        // start loading thread

        for (auto& file : sound_files)
        {
            auto loader = [this, file]() {
                getGame().getResourceManager().get().load<std::shared_ptr<soundEngineX::Buffer>, float&, float&>(
                    file, _percentCurrentResourceLoaded, _percentResourcesLoaded[file]);
            };
            _loaders[file] = std::move(loader);
        }
        auto loader = [this]() {
            std::ifstream fileStreamIn("../data/stars.txt", std::ios_base::binary);
            std::ifstream fileStreamIn2("../data/boys.txt", std::ios_base::binary);

            pg::generators::MarkovFrequencyMap<4> fmg;
            while (!fileStreamIn.eof() && fileStreamIn)
            {
                std::string word;
                fileStreamIn >> word;
                fmg.add(word);
            }
            _percentResourcesLoaded["stars.txt"] = 1.0f;
            _percentCurrentResourceLoaded = 0.5;

            while (!fileStreamIn2.eof() && fileStreamIn2)
            {
                std::string word;
                fileStreamIn2 >> word;
                fmg.add(word);
            }
            if (fmg.size() == 0)
            {
                fmg.add("empty");
                // log
                spdlog::error("No words loaded from file");
            }
            _percentResourcesLoaded["boys.txt"] = 1.0f;
            _percentCurrentResourceLoaded = 1.0f;
            getGame().addSingleton_as<pg::generators::MarkovFrequencyMap<4>>("markovFrequencyMap", fmg);
        };

        _loaders["markov"] = std::move(loader);

        _threads_running = std::make_unique<std::barrier<>>(_loaders.size() + 2);
        for (auto& [file, load_function] : _loaders)
        {
            createLoaderThread(std::move(load_function), file);
        }
        // start a watcher thread to update the progress
        createWatchProgressThread(_loaders.size());
        setupOverlay();
        // load resources for coming scene(s)

        Scene::start();
    };

    void postFrame(pg::FrameStamp& /*frameStamp*/) override
    {
        float totalProgress = getGame().getCurrentScene().getSingleton<float&>("resourceLoader.totalProgress");
        if (totalProgress >= 1.0f)
        {
            getGame().getGlobalDispatcher().trigger<pg::game::events::SwitchSceneEvent>({"galaxy"});
        }
    }

    void createLoaderThread(Loader&& loader, const std::string& resource)
    {
        std::jthread([this, loader = std::move(loader), resource] {
            try
            {
                loader();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            catch (const std::exception&)
            {
                spdlog::error("Failed to load resource: {}", resource);
            }
            _percentResourcesLoaded[resource] =
                1.0f; // might have been in cache already, thus not triggering a progress callback
            _percentCurrentResourceLoaded = 1.0f;
            _numRead++;
            std::unique_lock<std::mutex> lk(_mutex);
            _cv.notify_one();
            std::ignore = _threads_running->arrive();
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
                std::ignore = _threads_running->arrive();
            }).detach();
        }
        else
        {
            std::jthread([&, this, size] {
                while (_numRead < size)
                {
                    using namespace std::chrono_literals;
                    std::unique_lock<std::mutex> lk(_mutex);
                    _cv.wait_for(lk, 10ms);

                    auto percentTotalResourcesLoaded = 0.0;
                    for (const auto& [key, value] : _percentResourcesLoaded)
                    {
                        percentTotalResourcesLoaded += (value);
                    }
                    _percentTotalResourcesLoaded = percentTotalResourcesLoaded / _percentResourcesLoaded.size();
                }
                _percentTotalResourcesLoaded = 1.0f;
                std::ignore = _threads_running->arrive();
            }).detach();
        }
    }

    void setupOverlay()
    {
        // update events
        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiBegin>(), pg::game::DRAWABLE_FIRST});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiEnd>(), pg::game::DRAWABLE_LAST});
        pg::game::makeEntity<pg::game::GuiDrawable>(
            getSceneRegistry(),
            {std::make_unique<galaxy::gui::LoadResourcesWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});
    }

private:
    float _percentTotalResourcesLoaded{};

    float _percentCurrentResourceLoaded{};

    std::map<std::string, float> _percentResourcesLoaded{};

    std::atomic<uint32_t> _numRead{};
    Loaders               _loaders;
    // mutex and condition variable to signal when a resource is loaded

    std::mutex                      _mutex;
    std::condition_variable         _cv;
    std::unique_ptr<std::barrier<>> _threads_running;
    std::atomic_uint32_t            _numThreads{};
};
} // namespace galaxy