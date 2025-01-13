#pragma once

#include <functional>
#include <atomic>
#include <barrier>

class ResourcePreloader final
{
public:
    using UpdateTotalProgressCallback = std::function<void(float)>;
    ResourcePreloader() = default;

    ~ResourcePreloader()

    {
        if (_threads_running_barrier)
        {
            spdlog::info("Waiting for loading threads to finish");
            _threads_running_barrier->arrive_and_wait();
            spdlog::info("Loading threads finished");
        }
    }

public:
    void initialize(Loaders&                    loaders,
                    PercentCompletedMap&        completedMap,
                    UpdateTotalProgressCallback utc,
                    bool                        coarse_reporting = true)
    {
        // todo: check if already initialized
        _loaders = loaders;
        _coarse_reporting = coarse_reporting;
        _updateTotalProgressCallback = utc;

        // build thread threads and barrier
        _threads_running_barrier = std::make_unique<std::barrier<>>(_loaders.size() + 2);
        for (auto& [file, load_function] : _loaders)
        {
            createLoaderThread(std::move(load_function), file, completedMap);
        }
        // start a watcher thread to update the progress
        createWatchProgressThread(_loaders.size(), completedMap);
    }

    void createWatchProgressThread(std::vector<std::string>::size_type size, PercentCompletedMap& completedMap)
    {
        if (_coarse_reporting)
        {
            std::jthread([&, this, size] {
                while (_numRead < size)
                {
                    std::unique_lock<std::mutex> lk(_mutex_res_loaded);
                    _cv_res_loaded.wait(lk);

                    _updateTotalProgressCallback(static_cast<float>(_numRead) / size);
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
                    std::unique_lock<std::mutex> lk(_mutex_res_loaded);
                    _cv_res_loaded.wait_for(lk, 10ms);

                    auto percentTotalResourcesLoaded = 0.0f;
                    for (const auto& [key, value] : completedMap)
                    {
                        percentTotalResourcesLoaded += (value);
                    }
                    _updateTotalProgressCallback(percentTotalResourcesLoaded / completedMap.size());
                }
                _updateTotalProgressCallback(1.0f);
                std::ignore = _threads_running_barrier->arrive();
                _numThreadsFinished++;
            }).detach();
        }
    }

    void createLoaderThread(Loader&& loader, const std::string& resource, PercentCompletedMap& completedMap)
    {
        std::jthread([this, loader = std::move(loader), resource, &completedMap] {
            try
            {
                spdlog::info("Pre-Loading resource: {}", resource);
                loader(completedMap);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                spdlog::info("Pre-Loaded resource: {}. {} remaining", resource, _loaders.size() - _numThreadsFinished);
            }
            catch (const std::exception& e)
            {
                spdlog::error("Failed to load resource: {} ({})", resource, e.what());
                completedMap.insert({resource, 1.0f});
            }
            completedMap[resource] = 1.0f; // might have been in cache already, thus not triggering a progress callback
            _numRead++;

            std::unique_lock<std::mutex> lk(_mutex_res_loaded);
            _cv_res_loaded.notify_one();
            std::ignore = _threads_running_barrier->arrive();
            _numThreadsFinished++;
        }).detach();
    }

private:
    Loaders                         _loaders;                     // registered loaders to run
    bool                            _coarse_reporting{true};      // reporting progress for each step, else every 10ms
    std::atomic<uint32_t>           _numRead{};                   // number of resources loaded
    std::mutex                      _mutex_res_loaded;            // mutex for condition variable
    std::condition_variable         _cv_res_loaded;               // condition variable for resource loaded
    std::unique_ptr<std::barrier<>> _threads_running_barrier;     // barrier to wait for all threads to finish
    std::atomic_uint32_t            _numThreadsFinished{};        // number of threads finished
    UpdateTotalProgressCallback     _updateTotalProgressCallback; // callback to update total progress
};