#pragma once
#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

namespace pg {

struct FPSStats
{
    double averageFPS;
    double minFPS;
    double maxFPS;
};

class FPSCounter
{
public:
    FPSCounter()
      : frameCount(0)
      , totalTime(std::chrono::duration<double>::zero())
    {
    }

    void frame()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();

        if (frameCount > 0)
        {
            auto frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastFrameTime);
            totalTime += frameTime;

            double currentFPS = 1.0 / frameTime.count();
            fpsSamples.push_back(currentFPS);
            lastFrameFPS = currentFPS;
        }

        lastFrameTime = currentTime;
        frameCount++;
    }

    double getLastFrameFPS() const { return lastFrameFPS; }

    int getCurrentFrameCount() const { return frameCount; }

    FPSStats getAverageFPSAndReset()
    {
        FPSStats stats;

        if (frameCount > 1)
        {
            stats.averageFPS = frameCount / totalTime.count();
            stats.minFPS = *std::min_element(fpsSamples.begin(), fpsSamples.end());
            stats.maxFPS = *std::max_element(fpsSamples.begin(), fpsSamples.end());
        }
        else
        {
            // If only one frame is recorded, average, min, and max are the same
            stats.averageFPS = stats.minFPS = stats.maxFPS = (frameCount > 0) ? (1.0 / totalTime.count()) : 0.0;
        }

        // Reset internal statistics
        frameCount = 0;
        totalTime = std::chrono::duration<double>::zero();
        fpsSamples.clear();

        return stats;
    }

private:
    int                                            frameCount;
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    double                                         lastFrameFPS{60};
    std::chrono::duration<double>                  totalTime;
    std::vector<double>                            fpsSamples;
};

} // namespace pg

static std::ostream& operator<<(std::ostream& os, const pg::FPSStats& stats)
{
    os << std::setprecision(1) << std::fixed << "FPS Avg/Min/Max: " << stats.averageFPS << " / " << stats.minFPS
       << " / " << stats.maxFPS;
    return os;
}