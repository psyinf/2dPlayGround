#pragma once

namespace galaxy {

struct Lifetime
{
    uint64_t expected_lifetime{10000};
    float    reliability_factor{1.0}; // the lower the value, the more reliable the entity is
    uint64_t time_alive{0};
};

} // namespace galaxy