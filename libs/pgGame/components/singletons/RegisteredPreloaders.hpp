#pragma once
#include <map>

using PercentCompleted = std::map<std::string, float>;
using Loader = std::function<void(PercentCompleted&)>;

using Loaders = std::map<std::string, Loader>;

namespace pg::singleton {
struct RegisteredLoaders
{
    Loaders sync_loaders;
    Loaders loaders;
};
} // namespace pg::singleton
