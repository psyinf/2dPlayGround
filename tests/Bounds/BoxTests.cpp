#include <pgEngine/math/Box.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Construction", "[BoxTests]")
{
    pg::fBox b1{{0.0, 1.0}, {2.0, 3.0}};

    REQUIRE(b1.pos == pg::fVec2{0.0, 1.0});
    REQUIRE(b1.dim == pg::fVec2{2.0, 3.0});
}

TEST_CASE("Contains", "[BoxTests]")
{
    pg::fBox box1({0, 0}, {10, 10});
    pg::fBox box2({5, 5}, {2, 2});
    pg::fBox box3({-5, -5}, {2, 2});
    pg::fBox box4({5, 5}, {10, 10});

    REQUIRE(box1.contains(box2));
    REQUIRE_FALSE(box1.contains(box3));
    REQUIRE_FALSE(box1.contains(box4));
}

TEST_CASE("Intersects", "[BoxTests]")
{
    pg::fBox box1({0, 0}, {10, 10});
    pg::fBox box2({5, 5}, {2, 2});
    pg::fBox box3({-5, -5}, {2, 2});
    pg::fBox box4({5, 5}, {10, 10});
    pg::fBox box5({10, 10}, {2, 2});
    pg::fBox box6({9.99, 0}, {2, 2});
    REQUIRE(box1.intersects(box2));
    REQUIRE_FALSE(box1.intersects(box3));
    REQUIRE(box1.intersects(box4));
    REQUIRE_FALSE(box1.intersects(box5));
    REQUIRE(box1.intersects(box6));
}

TEST_CASE("Getters", "[BoxTests]")
{
    pg::fBox box({0, 0}, {10, 10});

    REQUIRE(box.left() == 0.0f);
    REQUIRE(box.right() == 10.0f);
    REQUIRE(box.top() == 0.0f);
    REQUIRE(box.bottom() == 10.0f);
    REQUIRE(box.midpoint() == pg::fVec2({5.0f, 5.0f}));
    REQUIRE(box.size() == pg::fVec2({10.0f, 10.0f}));
}