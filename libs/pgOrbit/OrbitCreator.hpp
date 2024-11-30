#pragma once
#include <pgOrbit/StarParameters.hpp>
#include <pgOrbit/OrbitalParameters.hpp>
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
            return randomBetween(0, 1) < 0.5 ? PlanetType::IceGiant : PlanetType::SuperEarth;
        }
        else
        {
            // Beyond snow line - gas giants or ice giants
            return randomBetween(0, 1) < 0.5 ? PlanetType::GasGiant : PlanetType::IceGiant;
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

    float randomBetween(float min, float max);

    double generateSemiMajorAxis(double luminosity)
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

    double generateLongitude() { return randomBetween(0.0, 360.0); }

    OrbitalParameters<double> generateOrbit(double luminosity, double frostLine)
    {
        OrbitalParameters<double> orbit;
        orbit.semimajor_axis = generateSemiMajorAxis(luminosity);
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
        std::random_device        rd;
        std::mt19937              gen(rd());
        std::gamma_distribution<> gamma_alpha(alpha, 1.0);
        std::gamma_distribution<> gamma_beta(beta, 1.0);

        double x = gamma_alpha(gen);
        double y = gamma_beta(gen);
        return std::min(x / (x + y), 0.15); // Cap eccentricity at 0.3
    }

    std::vector<OrbitalParams> generateOrbits(double luminosity, int numPlanets, double frostLine)
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
            orbit.inclination = randomBetween(0.0, std::numbers::pi / 18);  // Small inclination (0 to 10° in radians)
            orbit.longAN = randomBetween(0.0, 2 * std::numbers::pi);        // Random longitude of ascending node
            orbit.longPA = randomBetween(0.0, 2 * std::numbers::pi);        // Random longitude of periapsis
            orbit.meanLongitude = randomBetween(0.0, 2 * std::numbers::pi); // Random mean longitude

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

    std::vector<OrbitalParams> generateOrbits2(double luminosity, int numPlanets, double frostLine)
    {
        std::vector<OrbitalParams> orbits;

        double innerBoundary = 0.1;              // Minimum distance (AU) for the first orbit
        double outerBoundary = frostLine * 10.0; // Outer boundary based on frost line
        double minSpacingFactor = 0.2;           // Minimum spacing factor relative to current semi-major axis

        for (int i = 0; i < numPlanets; ++i)
        {
            if (innerBoundary >= outerBoundary)
            {
                std::swap(innerBoundary, outerBoundary);
                // Stop generating if there's no valid space left
                // std::cerr << "No more space for additional orbits. Stopping generation.\n";
                continue;
            }

            OrbitalParams newOrbit;
            bool          validOrbit = false;

            int maxAttempts = 100; // Limit the number of attempts to avoid endless loops
            int attempts = 0;

            while (!validOrbit && attempts < maxAttempts)
            {
                ++attempts;

                // Generate random semi-major axis within the defined boundaries
                newOrbit.semimajor_axis = randomBetween(innerBoundary, outerBoundary);

                // Generate other orbital parameters
                newOrbit.eccentricity = randomBetween(0.0, 0.1);                     // Low eccentricity for stability
                newOrbit.inclination = randomBetween(0.0, std::numbers::pi / 20);    // Small inclination in degrees
                newOrbit.longAN = randomBetween(0.0, 2.0 * std::numbers::pi);        // Random ascending node
                newOrbit.longPA = randomBetween(0.0, 2.0 * std::numbers::pi);        // Random argument of periapsis
                newOrbit.meanLongitude = randomBetween(0.0, 2.0 * std::numbers::pi); // Random mean longitude

                // Check for overlap with existing orbits
                validOrbit = true;
                for (const auto& orbit : orbits)
                {
                    double minSpacing = minSpacingFactor * orbit.semimajor_axis;
                    double innerLimit = orbit.semimajor_axis * (1 - orbit.eccentricity) - minSpacing;
                    double outerLimit = orbit.semimajor_axis * (1 + orbit.eccentricity) + minSpacing;

                    if (newOrbit.semimajor_axis * (1 + newOrbit.eccentricity) < innerLimit ||
                        newOrbit.semimajor_axis * (1 - newOrbit.eccentricity) > outerLimit)
                    {
                        continue;
                    }
                    else
                    {
                        validOrbit = false; // Overlap detected, regenerate this orbit
                        break;
                    }
                }
            }

            if (!validOrbit)
            {
                // Unable to place a new orbit after maxAttempts
                // std::cerr << "Failed to generate a valid orbit after " << maxAttempts << " attempts.\n";
                break;
            }

            // Add valid orbit to the system
            orbits.push_back(newOrbit);

            // Update innerBoundary for the next planet
            innerBoundary = newOrbit.semimajor_axis + minSpacingFactor * newOrbit.semimajor_axis;
        }

        return orbits;
    }

    void estimateLuminosity();

    void estimateMass();

    void calculateBasicParameters();

    float              _mass;
    float              _luminosity;
    OrbitCreatorConfig _config;
};
} // namespace pgOrbit