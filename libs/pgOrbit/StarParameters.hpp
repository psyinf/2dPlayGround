#pragma once
#include <array>
#include <magic_enum.hpp>
#include <cmath>

namespace pgOrbit {

using RGB = std::array<uint8_t, 3>;

template <typename T>
using StarClassParameter = std::array<T, 7>;

enum class SpectralType
{
    O,          //< Blue
    B,          //< Blue-white
    A,          //< White
    F,          //< Yellow-white
    G,          //< Yellow
    K,          //< Orange
    M,          //< Red
    BrownDwarf, //< Brown dwarf
    WhiteDwarf, //< White dwarf
    BlackHole,
    NeutronStar,
    Unknown,
};

inline auto spectralTypeToIndex(SpectralType type) -> std::size_t
{
    return magic_enum::enum_integer(type);
}

inline SpectralType indexToSpectralType(int index)
{
    return magic_enum::enum_cast<SpectralType>(index).value_or(SpectralType::Unknown);
}

static constexpr auto StarColors = std::array<RGB, 7>{
    RGB{155, 176, 255}, // O: Bluish white
    RGB{170, 191, 255}, // B: Blue-white
    RGB{202, 215, 255}, // A: White
    RGB{248, 247, 255}, // F: Yellow-white
    RGB{255, 244, 234}, // G: Yellow
    RGB{255, 210, 161}, // K: Orange
    RGB{255, 204, 111}  // M: Reddish-orange
};

// Lower and upper bounds of relative sizes for each spectral type
static constexpr auto StarLowerRelativeSizes = std::array<float, 7>{1.0f, 0.7f, 0.5f, 0.3f, 0.1f, 0.05f, 0.02f};
static constexpr auto StarUpperRelativeSizes = std::array<float, 7>{1.0f, 0.9f, 0.7f, 0.5f, 0.3f, 0.1f, 0.05f};

static constexpr auto StarLowerRelativeSunMasses = std::array<float, 7>{16.0f, 2.1f, 1.4f, 1.04f, 0.8f, 0.45f, 0.08f};
static constexpr auto StarUpperRelativeSunMasses = std::array<float, 7>{150.0f, 16.0f, 2.1f, 1.4f, 1.04f, 0.8f, 0.45f};
static constexpr auto star_class_probabilities =
    std::array<float, 7>{0.00003f, 0.001f, 0.007f, 0.03f, 0.08f, 0.12f, 0.75f};
// Lower and upper bounds of relative sizes for each spectral type

static constexpr auto perceivedBrightness = std::array<float, 7>{1.0f, 0.8f, 0.6f, 0.4f, 0.2f, 0.1f, 0.05f};

static inline float applyGamma(float brightness, float gamma)
{
    return std::powf(brightness, 1.0f / gamma);
}

// Function to convert a vector of brightness values by a gamma correction value
static constexpr std::array<float, 7> convertBrightnessByGamma(const std::array<float, 7>& brightness, float gamma)
{
    auto correctedBrightness = brightness;

    for (auto& value : correctedBrightness)
    {
        value = applyGamma(value, gamma);
    }

    return correctedBrightness;
}

template <typename T>
static constexpr T getForSpectralType(StarClassParameter<T> p, SpectralType type)
{
    return p[spectralTypeToIndex(type)];
}

} // namespace pgOrbit
