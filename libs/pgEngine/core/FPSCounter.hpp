#pragma once

#include <chrono>
#include <ostream>
#include <vector>

namespace pg {

struct FPSStats
{
    double averageFPS{};
    double minFPS{};
    double maxFPS{};
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

std::ostream& operator<<(std::ostream& os, const pg::FPSStats& stats);
