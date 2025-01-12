#pragma once
#include <unordered_map>

using PercentCompletedMap = std::unordered_map<std::string, float>;
using Loader = std::function<void(PercentCompletedMap&)>;

using Loaders = std::map<std::string, Loader>;

namespace pg::singleton {
struct RegisteredLoaders
{
    Loaders loaders;
};
} // namespace pg::singleton
