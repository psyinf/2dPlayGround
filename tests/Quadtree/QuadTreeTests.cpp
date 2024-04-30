#include <pgEngine/math/Box.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Quadtree Insertion and Range Query")
{
    // Create a bounding box representing the entire space
    pg::fBox rootBounds({0, 0}, {100, 100}); // Now the root bounds are defined with left, top, width, and height

    // Create a quadtree with the root bounds
    pg::Quadtree<int> quadtree(rootBounds);

    // Insert points into the quadtree
    quadtree.insert(pg::fBox({20, 40}, {10, 10}), 0, quadtree.root); // Point: left=20, top=40, width=10, height=10
    quadtree.insert(pg::fBox({70, 10}, {10, 10}), 1, quadtree.root); // Point: left=70, top=10, width=10, height=10

    SECTION("Range Query with Single Intersected Point")
    {
        // Define a query box
        pg::fBox queryBox({10, 30}, {30, 30}); // Query box: left=10, top=30, width=30, height=30

        // Perform range query and check intersected points
        auto intersections = quadtree.rangeQuery(queryBox);
        REQUIRE(intersections.size() == 1);
        REQUIRE(intersections[0].box.left() == 20);
        REQUIRE(intersections[0].box.top() == 40);
        REQUIRE(intersections[0].box.width() == 10);
        REQUIRE(intersections[0].box.height() == 10);
        REQUIRE(intersections[0].data.size() == 1);
        REQUIRE(intersections[0].data.at(0) == 0);
    }

    SECTION("Range Query with No Intersected Points")
    {
        // Define a query box
        pg::fBox queryBox({0, 0}, {10, 10}); // Query box: left=0, top=0, width=10, height=10

        // Perform range query and check intersected points
        auto intersections = quadtree.rangeQuery(queryBox);
        REQUIRE(intersections.empty());
    }

    SECTION("Range Query with Multiple Intersected Points")
    {
        // Define a query box covering the entire space
        pg::fBox queryBox({0, 0}, {100, 100}); // Query box: left=0, top=0, width=100, height=100

        // Perform range query and check intersected points
        auto intersections = quadtree.rangeQuery(queryBox);
        REQUIRE(intersections.size() == 2);
    }
}
