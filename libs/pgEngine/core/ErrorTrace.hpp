#pragma once
#include <iostream>
#ifdef __cpp_lib_stacktrace
#include <stacktrace>
#endif

namespace errorTrace {

inline void printErrorTrace()
{
#ifdef __cpp_lib_stacktrace
    auto&& trace = std::stacktrace::current();
    std::cerr << "Stacktrace:\n" << trace << std::endl;
#endif
}
} // namespace errorTrace
