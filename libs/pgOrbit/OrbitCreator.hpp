#pragma once
#include <pgOrbit/StarParameters.hpp>
#include <pgOrbit/OrbitalParameters.hpp>
#include <pgOrbit/Constants.hpp>
#include <random>
#include <numbers>

namespace pgOrbit {

class OrbitCreator
{
    struct OrbitCreatorConfig
    {
        pgOrbit::SpectralType spectralClass{pgOrbit::SpectralType::Unknown};
        double                scale{1.0};
        size_t                min_orbits{3};
        size_t                max_orbits{10};
        double                max_eccentricity{0.15};
        std::mt19937          gen{};
    };

    using OrbitalParams = OrbitalParameters<double>;

public:
    OrbitCreator(OrbitCreatorConfig&&);

    enum class PlanetType
    {
        Rocky,      // Terrestrial planets
        IceGiant,   // Ice planets, like Uranus/Neptune
        GasGiant,   // Jupiter-like planets
        SuperEarth, // Larger rocky planets
        HotJupiter  // Gas giants near the star
    };

    PlanetType determinePlanetType(double semiMajorAxis, double frostLine)
    {
        if (semiMajorAxis < frostLine)
        {
            // Inside frost line - likely rocky planets
            return PlanetType::Rocky;
        }
        else if (semiMajorAxis < 4.0)
        {
            // Between frost line and snow line - likely ice giants or super-Earths
            return randomBetween(0.0f, 1.0f) < 0.5f ? PlanetType::IceGiant : PlanetType::SuperEarth;
        }
        else
        {
            // Beyond snow line - gas giants or ice giants
            return randomBetween(0.0f, 1.0f) < 0.5f ? PlanetType::GasGiant : PlanetType::IceGiant;
        }
    }

    // TODO: outer structures, Oort cloud, Kuiper belt, asteroid belt, etc
    std::vector<std::pair<OrbitalParams, PlanetType>> generateSystem()
    {
        std::vector<std::pair<OrbitalParams, PlanetType>> system;

        double frostLine = 4.85 * std::sqrt(_luminosity); // Calculate frost line
        int    numPlanets =
            static_cast<int>(randomBetween(_config.min_orbits, _config.max_orbits)); // Random number of planets

        // Generate orbits without overlap
        auto orbits = generateOrbits(_luminosity, numPlanets, frostLine);

        // Assign planet types based on semi-major axis
        for (const auto& orbit : orbits)
        {
            PlanetType planetType = determinePlanetType(orbit.semimajor_axis, frostLine);
            system.push_back({orbit, planetType});
        }

        return system;
    }

private:
    pgOrbit::SpectralType getSpectralClass() const { return _config.spectralClass; }

    template <typename T>
    inline T randomBetween(T min, T max)
    {
        if (min > max) { std::swap(min, max); }
        // generator

        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<T> dis(min, max);
            return dis(_config.gen);
        }
        else
        {
            std::uniform_real_distribution<T> dis(min, max);
            return dis(_config.gen);
        }
    }

    double generateSemiMajorAxis_AU(double /*luminosity*/)
    {
        // TODO: configuration
        double innerLimit = 0.1;
        double outerLimit = 30.0;
        return _config.scale * std::pow(10, randomBetween(std::log10(innerLimit), std::log10(outerLimit)));
    }

    double generateEccentricity(double semiMajorAxis, double frostLine)
    {
        // TODO: configuration
        return (semiMajorAxis > frostLine) ? randomBetween(0.0, 0.4) : randomBetween(0.0, 0.2);
    }

    double generateInclination() { return randomBetween(0.0, 10.0); }

    double generateLongitude() { return randomBetween(-std::numbers::pi, std::numbers::pi); }

    OrbitalParameters<double> generateOrbit(double luminosity, double frostLine)
    {
        OrbitalParameters<double> orbit;
        orbit.semimajor_axis = generateSemiMajorAxis_AU(luminosity) * constants::AU;
        orbit.eccentricity = generateEccentricity(orbit.semimajor_axis, frostLine);
        orbit.inclination = generateInclination();
        orbit.longAN = generateLongitude();
        orbit.longPA = generateLongitude();
        orbit.meanLongitude = generateLongitude();
        return orbit;
    }

    // Function to generate a random eccentricity using a beta-like distribution
    double randomEccentricity()
    {
        double                    alpha = 1.5; // Shape parameter for more skew towards low values
        double                    beta = 30.0; // Shape parameter
        std::gamma_distribution<> gamma_alpha(alpha, 1.0);
        std::gamma_distribution<> gamma_beta(beta, 1.0);

        double x = gamma_alpha(_config.gen);
        double y = gamma_beta(_config.gen);
        return std::min(x / (x + y), _config.max_eccentricity); // Cap eccentricity at 0.15
    }

    std::vector<OrbitalParams> generateOrbits(double /*luminosity*/, int numPlanets, double frostLine)
    {
        std::vector<OrbitalParams> orbits;

        double innerBoundary = 0.1;            // Minimum distance (AU) for the first orbit
        double outerBoundary = frostLine * 10; // Maximum distance (AU) based on frost line

        double currentAxis = innerBoundary;
        for (int i = 0; i < numPlanets; ++i)
        {
            if (currentAxis >= outerBoundary)
            {
                // std::cerr << "Reached the outer boundary of the system. Stopping generation.\n";
                break;
            }

            OrbitalParams orbit;
            orbit.semimajor_axis = currentAxis;

            // Generate eccentricity using the beta-like distribution
            orbit.eccentricity = randomEccentricity();

            // Generate other orbital parameters
            orbit.inclination = generateLongitude() / 18.0; // Small inclination (0 to 10° in radians)
            orbit.longAN = generateLongitude();             // Random longitude of ascending node
            orbit.longPA = generateLongitude();             // Random longitude of periapsis
            orbit.meanLongitude = generateLongitude();      // Random mean longitude

            // Add orbit to the list
            orbits.push_back(orbit);

            // Calculate the next semi-major axis using a random spacing factor
            double spacingFactor = randomBetween(1.4, 1.8);
            currentAxis *= spacingFactor;
        }
        // apply scale
        for (auto& orbit : orbits)
        {
            orbit.semimajor_axis *= _config.scale;
        }
        return orbits;
    }

    void estimateRelLuminosity();

    void estimateRelMass();

    void calculateBasicParameters();

    float              _mass;
    float              _luminosity;
    OrbitCreatorConfig _config;
};
} // namespace pgOrbit
