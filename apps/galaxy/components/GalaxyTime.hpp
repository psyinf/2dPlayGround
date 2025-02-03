#pragma once

#include <chrono>

namespace galaxy {

struct StruturedTime
{
    std::chrono::seconds seconds;
    std::chrono::minutes minutes;
    std::chrono::hours   hours;
    std::chrono::days    days;
    std::chrono::months  months;
    std::chrono::years   years;

    StruturedTime fromTime()
    {
        auto secs = std::chrono::seconds(seconds);
        auto years = std::chrono::duration_cast<std::chrono::years>(secs);
        auto months = std::chrono::duration_cast<std::chrono::months>(secs) %
                      std::chrono::duration_cast<std::chrono::months>(std::chrono::years(1)).count();
        auto days = std::chrono::duration_cast<std::chrono::days>(secs) %
                    std::chrono::duration_cast<std::chrono::days>(std::chrono::months(1)).count();
        auto hours = std::chrono::duration_cast<std::chrono::hours>(secs) %
                     std::chrono::duration_cast<std::chrono::hours>(std::chrono::days(1)).count();

        auto minutes = std::chrono::duration_cast<std::chrono::hours>(secs) %
                       std::chrono::duration_cast<std::chrono::hours>(std::chrono::minutes(1)).count();

        return {secs, minutes, hours, days, months, years};
    }
};

//     static std::string format(const StruturedTime& structured_time) const
//     {
//         return std::format("{}.{:02}.{:02}.{:05}",
//                            structured_time.years.count(),
//                            structured_time.months.count(),
//                            structured_time.days.count(),
//                            structured_time.secs.count() %
//                                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days(1)).count());
//     }
} // namespace galaxy