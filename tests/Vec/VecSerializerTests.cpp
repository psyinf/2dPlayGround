#include <pgEngine/math/Vec.hpp>
#include <pgEngine/serializer/VecSerializer.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("serialize", "[VecSerializationTests]")
{
    pg::fVec2      v1{10.0, -5.2};
    nlohmann::json j = v1;
    auto           v2 = j.get<pg::fVec2>();
    REQUIRE(pg::equal(v1, v2));
}
