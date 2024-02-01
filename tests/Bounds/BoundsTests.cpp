#include <pgEngine/math/BoundsOperations.hpp>
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Miss", "[BoundsTests]")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform      t1{.pos{10, 0}};
    pg::Transform      t2{.pos{31, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) == std::nullopt);
}

TEST_CASE("Hit", "[BoundsTests]")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform      t1{.pos{10, 0}};
    pg::Transform      t2{.pos{29.9, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) != std::nullopt);
}

TEST_CASE("OnEdge", "[BoundsTests]")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform      t1{.pos{10, 0}};
    pg::Transform      t2{.pos{30.0, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) == std::nullopt);
}

TEST_CASE("Scaled Hit")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform      t1{.pos{10, 0}, .scale{1.1, 1.1}};
    pg::Transform      t2{.pos{30.1, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) != std::nullopt);
}

TEST_CASE("Scaled Miss")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform      t1{.pos{10, 0}, .scale{0.9, 0.9}};
    pg::Transform      t2{.pos{29.9, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) == std::nullopt);
}

TEST_CASE("Scaled max")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform      t1{.pos{10, 0}, .scale{1.0, 0.01}};
    pg::Transform      t2{.pos{29.9, 0}, .scale{1.0, 0.01}};

    REQUIRE(intersects(bs1, t1, bs2, t2) != std::nullopt);
}
