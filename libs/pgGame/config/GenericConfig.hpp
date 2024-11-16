#pragma once

#include <unordered_map>
#include <spdlog/spdlog.h>
#include <any>

namespace pg { namespace game {

struct GenericConfig
{
    std::unordered_map<std::string, std::any> config;

    template <class T>
    const T& getPerSceneConfig(const std::string& scene, const std::string& key) const
    {
        try
        {
            return std::any_cast<const T&>(config.at(scene + "::" + key));
        }
        catch (const std::out_of_range& /*e*/)
        {
            spdlog::warn("GenericConfig: Missing scene config '{}'", scene + "::" + key);
            static T t;
            return t;
        }
    }

    template <class T>
    T& getConfig(const std::string& key)
    {
        return std::any_cast<T&>(config[key]);
    }

    template <class T>
    void addConfig(const std::string& key, const T& value)
    {
        config[key] = value;
    }

    template <class T>
    void addPerSceneConfig(const std::string& scene, const std::string& key, const T& value)
    {
        config[scene + "::" + key] = value;
    }
};
}} // namespace pg::game