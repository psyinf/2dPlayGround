#include <pgf/caching/ResourceManager.hpp>
#include <catch2/catch_test_macros.hpp>

struct ResourceA
{
    std::string p;
};

struct ResourceB
{
    std::string p;
};

namespace pg::foundation {

template <>
auto loadResource<ResourceA>(const std::string& path) -> ResourceA
{
    if (path == "RES_C") { throw std::runtime_error("No loader for RES_C"); }
    return ResourceA{path};
}

template <>
ResourceA loadResource(const std::string& path, int x)
{
    return ResourceA{path + std::to_string(x)};
}
} // namespace pg::foundation

TEST_CASE("ResourceManager smoke tests", "[ResourceCache]")
{
    pg::foundation::ResourceManager<pg::foundation::MapppedResourceLocator> resMan(
        std::unordered_map<std::string, std::string>{{"resA", "RES_A"}, {"resB", "RES_B"}, {"resC", "RES_C"}});

    SECTION("Insert/Retrieve ResourceA")
    {
        auto result = resMan.load<ResourceA>("resA");
        REQUIRE(typeid(result) == typeid(std::shared_ptr<ResourceA>));
        REQUIRE(result->p == "RES_A");
        REQUIRE(result.use_count() == 2);

        auto result2 = resMan.load<ResourceA>("resA");
        REQUIRE(result.get() == result2.get());
        REQUIRE(result.use_count() == 3); // 2 shared_ptrs and 1 in cache
    }

    SECTION("missing locator")
    {
        REQUIRE_THROWS_AS(resMan.load<ResourceA>("resD"), std::runtime_error);
    }

    SECTION("missing loader")
    {
        REQUIRE_THROWS_AS(resMan.load<ResourceA>("resC"), std::runtime_error);
    }

    SECTION("parameter pack")
    {
        auto result2 = resMan.load<ResourceA, int>("resA", 5);
        REQUIRE(result2->p == "RES_A5");
    }
}
