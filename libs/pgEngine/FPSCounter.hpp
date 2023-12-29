#pragma once
#include <algorithm>
#include <chrono>
#include <iomanip>
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
    FPSCounter();

    void frame();

    std::chrono::milliseconds getLastFrameDuration() const;

    double getLastFrameFPS() const;

    size_t getCurrentFrameCount() const;

    FPSStats getAverageFPSAndReset();

private:
    size_t                                         frameCount;
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