#pragma once
#include <chrono>

namespace pg {
struct Time
{
    std::uint64_t milliseconds_accumulator{};
    std::uint64_t seconds{};

    void add(std::chrono::milliseconds ms, float time_scale)
    {
        milliseconds_accumulator += static_cast<uint64_t>(ms.count() * time_scale);
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

private:
    // get the frame duration in seconds
    float getFrameDuration_sec() const
    {
        auto f_secs = std::chrono::duration_cast<std::chrono::duration<float>>(lastFrameDuration);
        return f_secs.count();
    }

public:
    void update(std::chrono::milliseconds last_frame_duration, float time_scale)
    {
        frameNumber++;
        lastFrameDuration = last_frame_duration;
        previousTime = time;
        time.add(lastFrameDuration, time_scale);
    }

    // get the real time passed in seconds
    auto getRealTimePassed_sec() const { return time.getFractionalSeconds() - previousTime.getFractionalSeconds(); }

    auto formatTime()
    {
        auto seconds = time.seconds;

        auto secs = std::chrono::seconds(seconds);
        auto years = std::chrono::duration_cast<std::chrono::years>(secs);
        auto months = std::chrono::duration_cast<std::chrono::months>(secs) %
                      std::chrono::duration_cast<std::chrono::months>(std::chrono::years(1)).count();
        auto days = std::chrono::duration_cast<std::chrono::days>(secs) %
                    std::chrono::duration_cast<std::chrono::days>(std::chrono::months(1)).count();
        //         auto hours = std::chrono::duration_cast<std::chrono::hours>(secs) %
        //                      std::chrono::duration_cast<std::chrono::hours>(std::chrono::days(1)).count();
        return std::format("{}y.{:02}m.{:02}d.{:05}s",
                           years.count(),
                           months.count(),
                           days.count(),
                           secs.count() %
                               std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days(1)).count());
    }
};
} // namespace pg
