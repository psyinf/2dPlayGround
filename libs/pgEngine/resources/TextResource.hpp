#pragma once
#include <pgf/caching/ResourceManager.hpp>
#include <pgEngine/factories/Factories.hpp>
#include <pgf/caching/URI.hpp>

namespace pg {

struct TextResource
{
    pg::foundation::URI uri;
    std::vector<char>   text;

    std::string asString() { return std::string{text.begin(), text.end()}; }
};

namespace foundation {
template <>
inline TextResource loadResource<TextResource>(pg::foundation::DataProvider& p)
{
    return TextResource{p.getUri(), p.asBuffer()};
}
} // namespace foundation
} // namespace pg
