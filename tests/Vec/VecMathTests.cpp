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

    auto res3 = pg::swizzle(v, XYZ{});
    REQUIRE(res3 == Vec3<int>{1, 2, 3});

    auto res4 = pg::swizzle(v, RGBA{});
    REQUIRE(res4 == Vec4<int>{1, 2, 3, 4});

    using XZWY = pg::Swizzle<0, 2, 3, 1>;
    auto res5 = pg::swizzle(v, XZWY{});
    REQUIRE(res5 == Vec4<int>{1, 3, 4, 2});

    using XXXX = pg::Swizzle<0, 0, 0, 0>;
    auto res6 = pg::swizzle(v, XXXX{});
    REQUIRE(res6 == Vec4<int>{1, 1, 1, 1});

    //     using QQQQ = pg::Swizzle<3, 3, 3, 23>;
    //     auto res7 = pg::swizzle(v, QQQQ{});
    //     REQUIRE(res7 == Vec4<int>{4, 4, 4, 4});
}
