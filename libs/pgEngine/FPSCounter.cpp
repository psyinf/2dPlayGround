#include "FPSCounter.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

pg::FPSCounter::FPSCounter()
  : frameCount(0)
  , totalTime(std::chrono::duration<double>::zero())
{
}

void pg::FPSCounter::frame()
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

std::chrono::milliseconds pg::FPSCounter::getLastFrameDuration() const
{
    if (lastFrameFPS == 0.0)
    {
        return std::chrono::milliseconds(16); //
    }
    else
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<float>(1.0 / lastFrameFPS));
    }
}

double pg::FPSCounter::getLastFrameFPS() const
{
    return lastFrameFPS;
}

size_t pg::FPSCounter::getCurrentFrameCount() const
{
    return frameCount;
}

pg::FPSStats pg::FPSCounter::getAverageFPSAndReset()
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

static std::ostream& operator<<(std::ostream& os, const pg::FPSStats& stats)
{
    os << std::setprecision(1) << std::fixed << "FPS Avg/Min/Max: " << stats.averageFPS << " / " << stats.minFPS
       << " / " << stats.maxFPS;
    return os;
}