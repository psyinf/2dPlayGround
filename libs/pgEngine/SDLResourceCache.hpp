#pragma once
#include <any>
#include <sdlpp.hpp>
#include <string>
#include <unordered_map>

namespace pg {

class ResourceCache
{
public:
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
            resources[path] = std::make_shared<Resource>(std::move(maker(path, args...)));
        }
        return std::any_cast<std::shared_ptr<Resource>>(resources[path]);
    }

private:
    std::unordered_map<std::string, std::any> resources;
};
} // namespace pg
