#pragma once
#include <pgOrbit/FloatingPointType.hpp>

namespace pgOrbit {

template <FloatingPoint T = double>
struct OrbitalParameters
{
    // orbital parameters
    T eccentricity;
    T semimajor_axis;
    T inclination; ///> inclination of the orbit
    T longAN;      ///> longitude of ascending node
    T longPA;      ///> longitude of periapsis, basically the orientation of the ellipse
    /// time specific parameters
    T meanLongitude{}; /** Mean longitude is the ecliptic longitude at which an orbiting body could be found if its
                               orbit were circular and free of perturbations. While nominally a simple longitude, in
                               practice the mean longitude does not correspond to any one physical angle.**/
};

} // namespace pgOrbit
