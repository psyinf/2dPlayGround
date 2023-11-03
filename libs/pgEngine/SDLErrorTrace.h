#pragma once
#include <iostream>
#ifdef USE_STACKTRACES
#include <stacktrace>
#endif

namespace errorTrace {

inline void printErrorTrace()
{
#ifdef USE_STACKTRACES
    auto&& trace = std::stacktrace::current();
    std::cerr << "Stacktrace:\n" << trace << std::endl;
#endif
}
} // namespace errorTrace
