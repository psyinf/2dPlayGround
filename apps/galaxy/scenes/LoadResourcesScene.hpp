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

#include <pgGame/components/singletons/RegisteredPreloaders.hpp>

namespace galaxy {
// TODO: pre loader per scene-transition
class LoadResourcesScene : public pg::game::Scene
{
    static constexpr bool coarse = false;

public:
    LoadResourcesScene(pg::game::Game& game, pg::game::SceneConfig&& scene_cfg, std::string sceneIdToPrepareFor)
      : pg::game::Scene(game, std::move(scene_cfg))
      , _sceneIdToPrepareFor(sceneIdToPrepareFor)
    {
    }

    // using pg::game::Scene::Scene;

    virtual ~LoadResourcesScene()
    {
        // wait for threads to finish

        if (_threads_running_barrier)
        {
            spdlog::info("Waiting for loading threads to finish");
            _threads_running_barrier->arrive_and_wait();
            spdlog::info("Loading threads finished");
        }
    }

    void start() override
    {
        if (started()) { return; }
        // add percentage of resources loaded as singleton
        addSingleton_as<float&>("resourceLoader.totalProgress", _percentTotalResourcesLoaded);
        addSingleton_as<std::map<std::string, float>&>("resourceLoader.resourcesProgress", _percentResourcesLoaded);
        // get all registered loaders
        // TODO: this might be per scene
        auto& loaders = getGame().getSingleton<pg::singleton::RegisteredLoaders>(_sceneIdToPrepareFor + ".loaders");
        _loaders = loaders.loaders;
        // build thread threads and barrier
        _threads_running_barrier = std::make_unique<std::barrier<>>(_loaders.size() + 2);
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
            getGame().getGlobalDispatcher().trigger<pg::game::events::SwitchSceneEvent>({_sceneIdToPrepareFor});
        }
    }

    void createLoaderThread(Loader&& loader, const std::string& resource)
    {
        std::jthread([this, loader = std::move(loader), resource] {
            try
            {
                spdlog::info("Pre-Loading resource: {}", resource);
                loader(_percentResourcesLoaded);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                spdlog::info("Pre-Loaded resource: {}. {} remaining", resource, _loaders.size() - _numThreadsFinished);
            }
            catch (const std::exception&)
            {
                spdlog::error("Failed to load resource: {}", resource);
            }
            _percentResourcesLoaded[resource] =
                1.0f; // might have been in cache already, thus not triggering a progress callback
            _numRead++;
            std::unique_lock<std::mutex> lk(_mutex);
            _cv.notify_one();
            std::ignore = _threads_running_barrier->arrive();
            _numThreadsFinished++;
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
                std::ignore = _threads_running_barrier->arrive();
                _numThreadsFinished++;
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

                    auto percentTotalResourcesLoaded = 0.0f;
                    for (const auto& [key, value] : _percentResourcesLoaded)
                    {
                        percentTotalResourcesLoaded += (value);
                    }
                    _percentTotalResourcesLoaded = percentTotalResourcesLoaded / _percentResourcesLoaded.size();
                }
                _percentTotalResourcesLoaded = 1.0f;
                std::ignore = _threads_running_barrier->arrive();
                _numThreadsFinished++;
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

    PercentCompleted _percentResourcesLoaded{};

    std::atomic<uint32_t> _numRead{};
    Loaders               _loaders;
    // mutex and condition variable to signal when a resource is loaded

    std::mutex                      _mutex;
    std::condition_variable         _cv;
    std::unique_ptr<std::barrier<>> _threads_running_barrier;
    std::atomic_uint32_t            _numThreadsFinished{};
    std::string                     _sceneIdToPrepareFor;
};
} // namespace galaxy