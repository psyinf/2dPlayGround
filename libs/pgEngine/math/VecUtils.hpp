#pragma once
#include <pgEngine/math/Random.hpp>
#include <pgEngine/math/Vec.hpp>
#include <sdlpp.hpp>

#include <numeric>
#include <vector>

namespace pg {

template <typename Type>
constexpr Vec<Type, 2> dimsFromRect(const SDL_Rect& rect)
{
    return {static_cast<Type>(rect.w), static_cast<Type>(rect.h)};
}

template <typename Type>
constexpr Vec<Type, 2> startFromRect(const SDL_Rect& rect)
{
    return {static_cast<Type>(rect.x), static_cast<Type>(rect.y)};
}

// Function to split a vector into fragments that sum up to the original vector
static std::vector<pg::fVec2> splitVector(const pg::fVec2& original, int numFragments)
{
    std::vector<pg::fVec2> fragments;
    auto                   scale = length(original);
    for (int i = 0; i < numFragments - 1; ++i)
    {
        // Generate a random vector
        pg::fVec2 fragment = getRandomVector();

        while (dot(makeNormal(fragment), makeNormal(original)) > 0.999f)
        {
            fragment = getRandomVector();
        }

        // Scale the vector to ensure it sums up to the original vector
        fragment = fragment * (1.0f / numFragments) * scale;

        // Add the fragment to the vector of fragments
        fragments.push_back(fragment);
    }

    // The last fragment is the remaining portion needed to complete the original vector
    pg::fVec2 lastFragment = original - std::accumulate(fragments.begin(),
                                                        fragments.end(),
                                                        pg::fVec2{0, 0},
                                                        [](const auto& l, const auto& r) { return l + r; });
    fragments.push_back(lastFragment);
    return fragments;
}

} // namespace pg