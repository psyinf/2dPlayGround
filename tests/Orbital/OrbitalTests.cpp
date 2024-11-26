#include <pgOrbit/OrbitalMechanics.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

TEST_CASE("SmokeTests", "[OrbitalTests]")
{
    pgOrbit::OrbitalParameters<float> op{
        .eccentricity = 0.70,
        .semimajor_axis = 100,
        .incliniation = 0.0,
        .longAN = 0.0,
        .longPA = 0.0,
    };

    auto result = pgOrbit::OrbitalMechanics<float>::getEulerAnglesFromEccentricAnomaly(op, 0.0);

    REQUIRE(result.phi == 0.0);

    REQUIRE(result.radius == Catch::Approx(30.0f));
}
