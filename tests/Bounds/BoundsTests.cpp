#include <pgEngine/math/BoundsOperations.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Miss", "[BoundsTests]")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform2D    t1{.pos{10, 0}};
    pg::Transform2D    t2{.pos{31, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) == std::nullopt);
}

TEST_CASE("Hit", "[BoundsTests]")
{
    pg::BoundingSphere bs1{10.0f};
    pg::BoundingSphere bs2{10.0f};
    pg::Transform2D    t1{.pos{10.0f, 0}};
    pg::Transform2D    t2{.pos{29.9f, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) != std::nullopt);
}

TEST_CASE("OnEdge", "[BoundsTests]")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform2D    t1{.pos{10, 0}};
    pg::Transform2D    t2{.pos{30.0, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) == std::nullopt);
}

TEST_CASE("Scaled Hit")
{
    pg::BoundingSphere bs1{10.0f};
    pg::BoundingSphere bs2{10.0f};
    pg::Transform2D    t1{.pos{10.0f, 0.0f}, .scale{1.1f, 1.1f}};
    pg::Transform2D    t2{.pos{30.1f, 0.0f}};

    REQUIRE(intersects(bs1, t1, bs2, t2) != std::nullopt);
}

TEST_CASE("Scaled Miss")
{
    pg::BoundingSphere bs1{10.0};
    pg::BoundingSphere bs2{10.0};
    pg::Transform2D    t1{.pos{10.0f, 0}, .scale{0.9f, 0.9f}};
    pg::Transform2D    t2{.pos{29.9f, 0}};

    REQUIRE(intersects(bs1, t1, bs2, t2) == std::nullopt);
}

TEST_CASE("Scaled max")
{
    pg::BoundingSphere bs1{10.0f};
    pg::BoundingSphere bs2{10.0f};
    pg::Transform2D    t1{.pos{10.0f, 0}, .scale{1.0f, 0.01f}};
    pg::Transform2D    t2{.pos{29.9f, 0}, .scale{1.0f, 0.01f}};

    REQUIRE(intersects(bs1, t1, bs2, t2) != std::nullopt);
}
