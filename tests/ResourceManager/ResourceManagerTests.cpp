#include <pgFoundation/caching/ResourceManager.hpp>
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
auto loadResource<ResourceA>(const std::string& path)
{
    if (path == "RES_C") { throw std::runtime_error("ResourceA cannot be loaded from RES_C"); }

    return ResourceA{path};
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
}
