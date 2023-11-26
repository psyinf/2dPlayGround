#include <SDLVec.h>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Add", "[VecTests]")
{
   
    pg::fVec2 lhs{10.0, -5.2};
    pg::fVec2 rhs{-5.0, 2.0};
    pg::fVec2 expected_res{5.0, -3.2};
    pg::fVec2 res = lhs + rhs;
    REQUIRE(res[0] == Catch::Approx(expected_res[0]));
    REQUIRE(res[1] == Catch::Approx(expected_res[1]));
}

TEST_CASE("Sub", "[VecTests]")
{
    pg::fVec2 lhs{0.0, 5.2};
    pg::fVec2 rhs{5.0, 2.0};
    pg::fVec2 expected_res{-5.0, 3.2};
    pg::fVec2 res = lhs - rhs;
    REQUIRE(res[0] == Catch::Approx(expected_res[0]));
    REQUIRE(res[1] == Catch::Approx(expected_res[1]));
}

TEST_CASE("Dot", "[VecTests]")
{
    pg::fVec2 lhs{1.0, 5.2};
    pg::fVec2 rhs{5.0, 2.0};
    auto res = dot(lhs, rhs);
    REQUIRE(res == Catch::Approx(5.0f + 10.4f));
}

TEST_CASE("Length", "[VecTests]")
{
    pg::fVec2 lhs{1.0, 5.2};
    pg::fVec2 rhs{5.0, 2.0};
    auto      res = dot(lhs, rhs);
    REQUIRE(res == Catch::Approx(5.0f + 10.4f));
}