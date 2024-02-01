#include <pgGame/core/ResourceCache.hpp>
#include <catch2/catch_test_macros.hpp>


struct ResourceA
{
    std::string p;
};

struct ResourceB
{
    std::string p;
};

TEST_CASE("ResourceCache smoke tests", "[ResourceCache]")
{
    pg::ResourceCache cache{"xxx"};
    SECTION("Insert/Retrieve ResourceA"){
        auto resA = cache.load<ResourceA>("a", [](const auto& p) { return ResourceA{p}; });
        REQUIRE(typeid(resA) == typeid(std::shared_ptr<ResourceA>));
        REQUIRE(resA);
        REQUIRE(resA.use_count() == 2);
        REQUIRE(resA->p == "xxx/a");
        auto resA2nd = cache.load<ResourceA>("a", [](const auto& p) { return ResourceA{p}; });
        REQUIRE(resA2nd);
        REQUIRE(resA.use_count() == 3);
        REQUIRE(resA2nd.use_count() == 3);
      
        REQUIRE(resA2nd->p == "xxx/a");
        REQUIRE(resA.get() == resA2nd.get());
    }
    SECTION("Insert/Retrieve ResourceB")
    {
        auto resB = cache.load<ResourceB>("bb", [](const auto& p) { return ResourceB{p}; });
        REQUIRE(typeid(resB) == typeid(std::shared_ptr<ResourceB>));
        REQUIRE(resB);
        REQUIRE(resB.use_count() == 2);
        REQUIRE(resB->p == "xxx/bb");

    }
   
}

TEST_CASE("ResourceCache multiple", "[ResourceCache]")
{
    pg::ResourceCache cache{"uuu"};
    SECTION("Insert/Retrieve A and B")
    {
        auto resB = cache.load<ResourceB>("b", [](const auto& p) { return ResourceB{p}; });
        REQUIRE(typeid(resB) == typeid(std::shared_ptr<ResourceB>));
        REQUIRE(resB);
        REQUIRE(resB->p == "uuu/b");
        auto resB2 = cache.load<ResourceB>("aa", [](const auto& p) { return ResourceB{p}; });
        REQUIRE(typeid(resB2) == typeid(std::shared_ptr<ResourceB>));
        REQUIRE(resB2);
        REQUIRE(resB2->p == "uuu/aa");

    }
    SECTION("Re-register B with A's key")
    {
        auto resB = cache.load<ResourceB>("b", [](const auto& p) { return ResourceB{p}; });
        REQUIRE(typeid(resB) == typeid(std::shared_ptr<ResourceB>));
        REQUIRE(resB);
        REQUIRE(resB->p == "uuu/b");
        REQUIRE_THROWS_AS(cache.load<ResourceA>("b", [](const auto& p) { return ResourceA{p}; }), std::bad_any_cast);
       }
}