#pragma once
#include <chrono>

namespace pg {
struct Time
{
    std::uint64_t milliseconds_accumulator{};
    std::uint64_t seconds{};

    void add(std::chrono::milliseconds ms, float time_scale)
    {
        milliseconds_accumulator += ms.count() * time_scale;
        seconds += milliseconds_accumulator / 1000;
        milliseconds_accumulator %= 1000;
    }

    float getFractionalSeconds() const { return seconds + milliseconds_accumulator / 1000.0f; }
};

struct FrameStamp
{
    std::uint64_t             frameNumber{0};
    std::uint64_t             gameTick{0};
    std::chrono::milliseconds lastFrameDuration{0};

    Time time;
    Time previousTime;

    // get the frame duration in seconds
    float getFrameDuration_sec() const
    {
        auto f_secs = std::chrono::duration_cast<std::chrono::duration<float>>(lastFrameDuration);
        return f_secs.count();
    }

    void update(std::chrono::milliseconds last_frame_duration, float time_scale)
    {
        frameNumber++;
        lastFrameDuration = last_frame_duration;
        previousTime = time;
        time.add(lastFrameDuration, time_scale);
    }

    // get the real time passed in seconds
    auto getRealTimePassed() const { return time.getFractionalSeconds() - previousTime.getFractionalSeconds(); }
};
} // namespace pg