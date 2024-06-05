#include <pgEngine/math/Vec.hpp>
#include <pgEngine/math/VecSerializer.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("serialize", "[VecSerializationTests]")
{
    pg::fVec2      v1{10.0, -5.2};
    nlohmann::json j = v1;
    auto           v2 = j.get<pg::fVec2>();
    REQUIRE(pg::equal(v1, v2));
}

TEST_CASE("serializeVecN", "[VecSerializationTests]")
{
    pg::Vec<int, 100> v1;
    std::iota(v1.begin(), v1.end(), 0);

    nlohmann::json j = v1;
    auto           v2 = j.get<pg::Vec<int, 100>>();
    REQUIRE(pg::equal(v1, v2));
}
