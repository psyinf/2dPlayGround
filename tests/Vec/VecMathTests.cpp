#include <pgEngine/math/Vec.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <numbers>

TEST_CASE("angleBetween", "[VecTests]")
{
    using namespace pg;
    fVec2 ref{0, 1};
    auto  data = GENERATE(table<pg::fVec2, float>({
        {{0, 1}, 0.0f},
        {{1, 0}, -std::numbers::pi / 2.0f},
        {{0, -1}, -std::numbers::pi},
        {{-1, 0}, std::numbers::pi / 2.0f},
    }));

    auto res = angleBetween(ref, std::get<0>(data));
    REQUIRE(res == Catch::Approx(std::get<1>(data)));
}
