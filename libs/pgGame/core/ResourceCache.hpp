#pragma once
#include <any>
#include <sdlpp.hpp>
#include <string>
#include <unordered_map>

namespace pg {

class ResourceCache
{
public:
    ResourceCache(const std::string& basePath)
      : basePath(basePath)
    {
    }

    template <typename Resource>
    std::shared_ptr<Resource> load(const std::string& path)
    {
        if (resources.find(path) == resources.end()) { resources[path] = std::make_shared<Resource>(path); }
        return std::any_cast<std::shared_ptr<Resource>>(resources[path]);
    }

    template <typename Resource, typename Maker, typename... Args>
    std::shared_ptr<Resource> load(const std::string& path, Maker&& maker, Args... args)
    {
        if (resources.find(path) == resources.end())
        {
            // TODO: use std::filesystem
            resources[path] = std::make_shared<Resource>(std::move(maker(basePath + "/" + path, args...)));
        }
        return std::any_cast<std::shared_ptr<Resource>>(resources[path]);
    }

private:
    std::unordered_map<std::string, std::any> resources;
    std::string                               basePath;
};

template <typename Resource, typename Maker = std::function<Resource(const std::string&)>>
class TypedResourceCache
{
public:
    TypedResourceCache(const std::string& basePath, Maker&& maker)
      : basePath(basePath)
      , maker(maker)
    {
    }


    std::shared_ptr<Resource> load(const std::string& path)
    {
        if (resources.find(path) == resources.end())
        {
            // TODO: use std::filesystem
            resources[path] = std::make_shared<Resource>(std::move(maker(basePath + "/" + path)));
        }
        return resources[path];
    }

private:
    std::string                                                basePath;
    std::unordered_map<std::string, std::shared_ptr<Resource>> resources;
    Maker                                                      maker;
};
} // namespace pg
