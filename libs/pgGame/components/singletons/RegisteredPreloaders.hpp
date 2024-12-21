#pragma once
#include <unordered_map>

using PercentCompleted = std::unordered_map<std::string, float>;
using Loader = std::function<void(PercentCompleted&)>;

using Loaders = std::map<std::string, Loader>;

namespace pg::singleton {
struct RegisteredLoaders
{
    Loaders loaders;
};
} // namespace pg::singleton
