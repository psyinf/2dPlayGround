#pragma once
#include <chrono>
namespace game {
struct FrameStamp
{
    std::uint64_t             frameNumber;
    std::chrono::milliseconds lastFrameDuration;
};
} // namespace game