#pragma once
#include <pgOrbit/FloatingPointType.hpp>
#include <pgEngine/math/Vec.hpp>

namespace pgOrbit {

template <FloatingPoint T>
struct EulerCoordinates
{
    double phi;
    double theta;
    double psi;
    double radius;

    pg::Vec3<T> toCartesian() const
    {
        T x = radius * (cos(phi) * cos(theta) - sin(phi) * cos(psi) * sin(theta));
        T y = radius * (sin(phi) * cos(theta) * cos(psi) + cos(phi) * sin(theta));
        T z = radius * (sin(phi) * sin(psi));
        return {x, y, z};
    }
};
} // namespace pgOrbit