#pragma once

namespace asteroids {
//
struct Sound
{
    std::string identifier; //< usually the path or some resource identifier
    float       gain{1.0f}; //< range 0.0f to 1.0f
    float       pitch{1.0f};
    bool        loop{false};
} // namespace asteroids