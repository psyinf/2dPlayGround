#pragma once

#include <nlohmann/json.hpp>

#include <any>

namespace pg { namespace game {

using ConfigItem = nlohmann::json;

struct GenericConfig
{
    std::unordered_map<std::string, std::any> config;

    template <class T>
    T& getPerSceneConfig(const std::string scene, const std::string& key)
    {
        return std::any_cast<T&>(config[key]);
    }

    template <class T>
    T& getConfig(const std::string& key)
    {
        return std::any_cast<T&>(config[key]);
    }
};
}} // namespace pg::game