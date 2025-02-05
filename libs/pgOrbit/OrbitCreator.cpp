#include "OrbitCreator.hpp"

pgOrbit::OrbitCreator::OrbitCreator(OrbitCreatorConfig&& config)
  : _config(std::move(config))
{
    calculateBasicParameters();
}

void pgOrbit::OrbitCreator::estimateRelLuminosity()
{
    if (_mass < 0.43f) { _luminosity = 0.23f * std::pow(_mass, 2.3f); }
    else if (_mass <= 2.0f) { _luminosity = std::pow(_mass, 4.0f); }
    else { _luminosity = 1.5f * std::pow(_mass, 3.5f); }
}

void pgOrbit::OrbitCreator::estimateRelMass()
{
    auto lowerMass = pgOrbit::getForSpectralType(pgOrbit::StarLowerRelativeSunMasses, getSpectralClass());
    auto upperMass = pgOrbit::getForSpectralType(pgOrbit::StarUpperRelativeSunMasses, getSpectralClass());
    _mass = randomBetween(lowerMass, upperMass);
}

void pgOrbit::OrbitCreator::calculateBasicParameters()
{
    estimateRelMass();
    estimateRelLuminosity();
}
