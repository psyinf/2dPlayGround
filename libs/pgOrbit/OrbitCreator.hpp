#pragma once
#include <pgOrbit/StarParameters.hpp>
#include <pgOrbit/OrbitalParameters.hpp>
#include <random>

namespace pgOrbit {

class OrbitCreator
{
public:
    OrbitCreator(pgOrbit::SpectralType spectralClass);

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

private:
    float randomBetween(float min, float max);

    double generateSemiMajorAxis(double luminosity)
    {
        double innerLimit = 0.1;
        double outerLimit = 30.0;
        return std::pow(10, randomBetween(std::log10(innerLimit), std::log10(outerLimit)));
    }

    double generateEccentricity(double semiMajorAxis, double frostLine)
    {
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

    std::vector<OrbitalParameters<double>> generateSystem()
    {
        std::vector<OrbitalParameters<double>> system;

        // Determine frost line and habitable zone
        double frostLine = 4.85 * std::sqrt(_luminosity);
        int    numPlanets = static_cast<int>(randomBetween(1, 10)); // Adjust range based on type

        // Generate planets
        for (int i = 0; i < numPlanets; ++i)
        {
            system.push_back(generateOrbit(_luminosity, frostLine));
        }

        return system;
    }

    void estimateLuminosity();

    void estimateMass();

    void calculateBasicParameters();

    float                 _mass;
    float                 _luminosity;
    pgOrbit::SpectralType _spectralClass;
};
} // namespace pgOrbit