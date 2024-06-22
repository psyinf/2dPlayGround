#pragma once
#include <chrono>

namespace pg::game {
struct FrameStamp
{
    std::uint64_t             frameNumber{0};
    std::uint64_t             gameTick{0};
    std::chrono::milliseconds lastFrameDuration{0};

    float getFrameDuration_sec() const
    {
        auto f_secs = std::chrono::duration_cast<std::chrono::duration<float>>(lastFrameDuration);
        return f_secs.count();
    }
};
} // namespace pg::game