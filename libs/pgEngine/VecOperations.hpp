#pragma once
#include <SDLVec.h>
#include <numeric>
#include <random>
#include <vector>

namespace pg {
// Function to generate a random vector
pg::fVec2 getRandomVector()
{
    static std::random_device                    rd;
    static std::mt19937                          gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    return makeNormal(pg::fVec2{dis(gen), dis(gen)});
}

// Function to split a vector into fragments
// TODO: optimize
std::vector<pg::fVec2> splitVector(const pg::fVec2& original, int numFragments)
{
    std::vector<pg::fVec2> fragments;

    for (int i = 0; i < numFragments - 1; ++i)
    {
        // Generate a random vector
        pg::fVec2 fragment = getRandomVector();
        // Ensure the generated vector is not collinear
        while (std::abs(fragment[0] * original[1] - fragment[1] * original[0]) < 1e-6)
        {
            fragment = getRandomVector();
        }

        // Scale the vector to ensure it sums up to the original vector
        fragment = fragment * (1.0f / numFragments);
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