#include <pgEngine/math/Vec.hpp>
#include <pgEngine/math/VecOps.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>

TEST_CASE("Add", "[VecTests]")
{
    pg::fVec2 lhs{10.0f, -5.2f};
    pg::fVec2 rhs{-5.0f, 2.0f};
    pg::fVec2 expected_res{5.0f, -3.2f};
    pg::fVec2 res = lhs + rhs;
    REQUIRE(res[0] == Catch::Approx(expected_res[0]));
    REQUIRE(res[1] == Catch::Approx(expected_res[1]));
}

TEST_CASE("Sub", "[VecTests]")
{
    pg::fVec2 lhs{0.0f, 5.2f};
    pg::fVec2 rhs{5.0f, 2.0f};
    pg::fVec2 expected_res{-5.0f, 3.2f};
    pg::fVec2 res = lhs - rhs;
    REQUIRE(res[0] == Catch::Approx(expected_res[0]));
    REQUIRE(res[1] == Catch::Approx(expected_res[1]));
}

TEST_CASE("Dot", "[VecTests]")
{
    pg::fVec2 lhs{1.0f, 5.2f};
    pg::fVec2 rhs{5.0f, 2.0f};
    auto      res = pg::dot(lhs, rhs);
    REQUIRE(res == Catch::Approx(5.0f + 10.4f));
}

TEST_CASE("LengthSquared", "[VecTests]")
{
    pg::fVec2 lhs{3.0f, 4.0f};

    auto res = pg::lengthSquared(lhs);
    REQUIRE(res == Catch::Approx(25.0f));
    REQUIRE(res == pg::dot(lhs, lhs));
}

TEST_CASE("Length", "[VecTests]")
{
    pg::fVec2 lhs{3.0f, 4.0f};
    auto      res = pg::length(lhs);
    REQUIRE(res == Catch::Approx(5.0f));
    REQUIRE(res == std::sqrt(pg::dot(lhs, lhs)));
}