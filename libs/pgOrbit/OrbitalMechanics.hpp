#pragma once
#include <cmath>
#include <pgOrbit/EulerCoordinates.hpp>
#include <pgOrbit/OrbitalParameters.hpp>

namespace pgOrbit {

template <FloatingPoint T>
class OrbitalMechanics
{
private:
    static T getEccentricAnomaly(T mean_anomaly, T eccentricity)
    {
        T e = 0.0;
        T lastDelta = 2.0;
        for (T delta = 1.0, e_ = mean_anomaly; delta < lastDelta;)
        {
            e = eccentricity * std::sin(e_) + mean_anomaly;
            lastDelta = delta;
            delta = std::abs(e_ - e);
            e_ = e;
        }
        return e;
    }

    /**/
    static T getTrueAnomaly(T eccentricAnomaly, T eccentricity)
    {
        return 2.0 * std::atan2(                                                           //
                         std::sqrt(1.0 + eccentricity) * std::sin(eccentricAnomaly / 2.0), //
                         std::sqrt(1.0 - eccentricity) * std::cos(eccentricAnomaly / 2.0)  //
                     );
    }

    static T getRadius(const OrbitalParameters<T>& op, T eccentricAnomaly)
    {
        return op.semimajor_axis * (1.0 - op.eccentricity * std::cos(eccentricAnomaly));
    }

    static T getArgumentOfPeriapsis(const OrbitalParameters<T>& op) { return op.longPA - op.longAN; }

    static T getMeanAnomaly(const OrbitalParameters<T>& op) { return op.meanLongitude - op.longPA; };

public:
    static EulerCoordinates<T> getEulerAnglesFromEccentricAnomaly(const OrbitalParameters<T>& op, T eccentricAnomaly)
    {
        T trueAnomaly = getTrueAnomaly(eccentricAnomaly, op.eccentricity);
        T argumentOfPeriapsis = getArgumentOfPeriapsis(op);

        T psi = trueAnomaly + argumentOfPeriapsis;

        T r = getRadius(op, eccentricAnomaly);

        return EulerCoordinates<T>{psi, op.longAN, op.incliniation, r};
    }

    // TODO: this is a simplification over a orbit derived from an object "falling" around a central mass and having its
    // speed derived from
    static auto getEulerAngelsAtJulianDay(const OrbitalParameters<T>& op, T day)
    {
        // orbital period T = 2PI*sqrt(a^3/GM), a == semi-major axis
        auto siderealOrbitPeriod = 365.0;
        T    meanMotion = 2.0 * std::atan(1.0) * 4.0 / siderealOrbitPeriod;
        T    currentMeanAnomaly = getMeanAnomaly(op) + meanMotion * day; //(day == jd - m_epoch;)

        T eccentricAnomaly = getEccentricAnomaly(currentMeanAnomaly, op.eccentricity);

        return getEulerAnglesFromEccentricAnomaly(op, eccentricAnomaly);
    }

    /**
     * Get the angle on the orbit at a given percentage of the orbital period
     */

    static auto getEulerAngelsAtFraction(const OrbitalParameters<T>& op, T period_fraction)
    {
        // orbital period T = 2PI*sqrt(a^3/GM), a == semi-major axis

        T meanMotion = 2.0 * std::atan(1.0) * 4.0;
        T currentMeanAnomaly = getMeanAnomaly(op) + meanMotion * period_fraction;

        T eccentricAnomaly = getEccentricAnomaly(currentMeanAnomaly, op.eccentricity);

        return getEulerAnglesFromEccentricAnomaly(op, eccentricAnomaly);
    }
};
} // namespace pgOrbit