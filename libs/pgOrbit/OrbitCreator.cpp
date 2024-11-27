#include "OrbitCreator.hpp"
#include <random>

pgOrbit::OrbitCreator::OrbitCreator(SpectralType spectralClass)
  : _spectralClass(spectralClass)
{
}

float pgOrbit::OrbitCreator::randomBetween(float min, float max)
{
    // generator
    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

void pgOrbit::OrbitCreator::estimateLuminosity()
{
    if (_mass < 0.43f) { _luminosity = 0.23f * std::pow(_mass, 2.3f); }
    else if (_mass <= 2.0f) { _luminosity = std::pow(_mass, 4.0f); }
    else { _luminosity = 1.5f * std::pow(_mass, 3.5f); }
}

void pgOrbit::OrbitCreator::estimateMass()
{
    auto lowerMass = pgOrbit::getForSpectralType(pgOrbit::StarLowerRelativeSunMasses, _spectralClass);
    auto upperMass = pgOrbit::getForSpectralType(pgOrbit::StarUpperRelativeSunMasses, _spectralClass);
    _mass = randomBetween(lowerMass, upperMass);
}

void pgOrbit::OrbitCreator::calculateBasicParameters() {}
