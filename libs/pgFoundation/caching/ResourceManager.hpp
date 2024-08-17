#pragma once

#include <memory>
#include <string>
#include <pgFoundation/caching/ResourceCache.hpp>
#include <pgFoundation/caching/ResourceLocator.hpp>

namespace pg::foundation {

template <typename T>
inline auto loadResource(const std::string& path) -> T
{
    static_assert(false, "No resource loader found for type T");
}

template <typename T, typename... Args>
inline auto loadResource(const std::string& path, Args... args) -> T
{
    static_assert(false, "No resource loader found for type T");
}

template <typename Locator>
class ResourceManager
{
public:
    ResourceManager() = default;

    ResourceManager(Locator&& locator)
      : _locator(locator)
    {
    }

    template <class T>
    std::shared_ptr<T> load(const std::string& uri)
    {
        if (!_locator.contains(uri)) { throw std::runtime_error("Locator does not contain uri"); }
        auto path = (_locator.locate(uri)).string();
        return _cache.retrieve<T>(uri, [path]([[maybe_unused]] const std::string& _) {
            return std::move(pg::foundation::loadResource<T>(path));
        });
    }

    template <class T, typename... Args>
    std::shared_ptr<T> load(const std::string& uri, Args&&... args)
    {
        if (!_locator.contains(uri)) { throw std::runtime_error("Locator does not contain uri"); }
        auto path = (_locator.locate(uri)).string();
        return _cache.retrieve<T>(uri, [path, &args...]([[maybe_unused]] const std::string& _) {
            return pg::foundation::loadResource<T, Args...>(path, std::forward<Args>(args)...);
        });
    }

    template <class T>
    void put(const std::string& uri, T&& resource)
    {
        auto path = (_locator.locate(uri)).string();
        _cache.put<T>(uri, std::forward<T&&>(resource));
    }

    template <class T>
    void put(const std::string& uri, std::shared_ptr<T> resource)
    {
        auto path = (_locator.locate(uri)).string();
        _cache.put<T>(uri, resource);
    }

    template <class T>
    std::shared_ptr<T> get(const std::string& uri)
    {
        if (!_locator.contains(uri)) { throw std::runtime_error("Locator does not contain uri"); }
        auto path = (_locator.locate(uri)).string();
        return _cache.get<T>(path);
    }

    Locator& getLocator() { return _locator; }

private:
    Locator                       _locator;
    pg::foundation::ResourceCache _cache;
};

template <typename Locator>
class ResourceManagerMonostate
{
public:
    ResourceManager<Locator>& get() { return instance; };

private:
    static inline ResourceManager<Locator> instance{};
};
} // namespace pg::foundation