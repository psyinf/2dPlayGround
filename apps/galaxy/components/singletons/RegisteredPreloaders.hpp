#pragma once
#include <map>

using PercentCompleted = std::map<std::string, float>;
using Loader = std::function<void(PercentCompleted&)>;

using Loaders = std::map<std::string, Loader>;

namespace galaxy::singleton {
struct RegisteredLoaders
{
    Loaders loaders;
};
} // namespace galaxy::singleton
