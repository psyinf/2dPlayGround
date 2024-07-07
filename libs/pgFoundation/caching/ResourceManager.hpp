#pragma once

#include <memory>
#include <pgFoundation/caching/ResourceCache.hpp>
#include <pgFoundation/caching/ResourceLocator.hpp>

namespace pg::foundation {
template <typename T, typename... Args>
auto loadResource(const std::filesystem::path& path, Args... args)
{
    static_assert(false, "No resource loader found for type T");
}

template <typename Locator>
class ResourceManager
{
public:
    ResourceManager(Locator locator)
      : _locator(locator)
    {
    }

    // we need
    //  a resource locator translating an URI to a path
    //  a factory that creates resource objects from paths
    //  a cache that stores the resources and manages their lifetime
    //     template <typename... Args>
    //     std::shared_ptr<T> load(const std::string& uri, Args... args)
    //     {
    //         auto path = _locator.locate(uri);
    //         _cache.retrieve(uri, [path](const auto& e) { return load(path, args...); });
    //     }
    template <class T>
    std::shared_ptr<T> load(const std::string& uri)
    {
        auto path = (_locator.locate(uri)).string();
        if (_cache.has(path)) { return _cache.get<T>(path); }
        else
        {
            return _cache.retrieve<T>(path, [path]() { return std::move(pg::foundation::loadResource<T>(path)); });
        }
    }

    //     template <class T, typename... Args>
    //     std::shared_ptr<T> load(const std::string& uri, Args... args)
    //     {
    //         auto path = (_locator.locate(uri)).string();
    //         if (_cache.has(path)) { return _cache.get<T>(path); }
    //         else
    //         {
    //             return _cache.retrieve<T>(path, [path]() {
    //                 return std::move(pg::foundation::loadResource<T>(path, std::forward<Args>(args)...));
    //             });
    //         }
    //     }

private:
    Locator                       _locator;
    pg::foundation::ResourceCache _cache;
};

} // namespace pg::foundation