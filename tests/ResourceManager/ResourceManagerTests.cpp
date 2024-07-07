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
auto loadResource<ResourceA>(const std::filesystem::path& path)
{
    return ResourceA{path.string()};
}
} // namespace pg::foundation

TEST_CASE("ResourceManager smoke tests", "[ResourceCache]")
{
    pg::foundation::ResourceManager<pg::foundation::MapppedResourceLocator> resMan({});
    SECTION("Insert/Retrieve ResourceA")
    {
        resMan.load<ResourceA>("resA");
    }
}
