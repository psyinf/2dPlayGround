#include <pgEngine/math/Vec.hpp>
#include <pgEngine/math/VecOps.hpp>
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
        {{0, -1}, static_cast<float>(-std::numbers::pi)},
        {{-1, 0}, std::numbers::pi / 2.0f},
    }));

    auto res = pg::angleBetween(ref, std::get<0>(data));
    REQUIRE(res == Catch::Approx(std::get<1>(data)));
}

TEST_CASE("swizzle", "[VecTests]")
{
    using namespace pg;
    auto v = Vec4<int>{1, 2, 3, 4};

    auto res = pg::sub<0, 0>(v);
    REQUIRE(res == Vec2<int>{1, 1});
}
