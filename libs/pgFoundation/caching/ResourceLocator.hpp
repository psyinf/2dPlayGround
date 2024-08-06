#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace pg::foundation {
template <typename T>
class ResourceLocatorBase
{
public:
    std::filesystem::path locate(const std::string& uri) { return static_cast<T&>(*this).loc_impl(uri); }

    bool contains(const std::string& uri) const { return (static_cast<const T&>(*this)).has_impl(uri); }
};

class IdentityResourceLocator : public ResourceLocatorBase<IdentityResourceLocator>
{
public:
    std::filesystem::path loc_impl(const std::string& uri) { return uri; }

    bool has_impl([[maybe_unused]] const std::string& uri) const { return true; }
};

class MapppedResourceLocator : public ResourceLocatorBase<MapppedResourceLocator>
{
public:
    using UriMap = std::unordered_map<std::string, std::string>;

    MapppedResourceLocator(UriMap&& locations)
      : _locations(std::move(locations))
    {
    }

    void addLocation(const std::string& uri, const std::string& path) { _locations[uri] = path; }

    std::string loc_impl(const std::string& uri) { return _locations.at(uri); }

    bool has_impl(const std::string& uri) const { return _locations.contains(uri); }

private:
    UriMap _locations;
};

// resource locator that constructs a path from the URI
class PathResourceLocator : public ResourceLocatorBase<PathResourceLocator>
{
public:
    PathResourceLocator(const std::filesystem::path& basePath)
      : _basePath(basePath)
    {
    }

    bool has_impl(const std::string& uri) const
    {
        return std::filesystem::exists(_basePath / std::filesystem::path{uri});
    }

    std::filesystem::path loc_impl(const std::string& uri) { return _basePath / std::filesystem::path{uri}; }

private:
    std::filesystem::path _basePath;
};

} // namespace pg::foundation
