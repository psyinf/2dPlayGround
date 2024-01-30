#pragma once
#include <chrono>

namespace pg::game {
struct FrameStamp
{
    std::uint64_t             frameNumber;
    std::chrono::milliseconds lastFrameDuration;

    float getFrameDuration_sec() const
    {
        auto f_secs = std::chrono::duration_cast<std::chrono::duration<float>>(lastFrameDuration);
        return f_secs.count();
    }
};
} // namespace pg::game