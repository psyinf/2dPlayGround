#pragma once

#include <unordered_map>
#include <spdlog/spdlog.h>
#include <any>

namespace pg { namespace game {

class GenericConfig
{
private:
    mutable std::unordered_map<std::string, std::any> config;

public:
    template <class T>
    const T& getPerContextConfig(const std::string& ctx, const std::string& key) const
    {
        try
        {
            return std::any_cast<const T&>(config.at(ctx + "::" + key));
        }
        catch (const std::out_of_range& /*e*/)
        {
            spdlog::warn("GenericConfig: Missing context config '{}'", ctx + "::" + key);
            static T t;
            return t;
        }
    }

    template <class T>
    const T& getConfig(const std::string& key, T&& default_value = T{}) const
    {
        try
        {
            return std::any_cast<T&>(config.at(key));
        }
        catch (const std::out_of_range& /*e*/)
        {
            spdlog::warn("GenericConfig: Missing config item '{}', returning default", key);

            config[key] = default_value;
            return std::any_cast<T&>(config.at(key));
        }
    }

    template <class T>
    T& getConfig(const std::string& key, T&& default_value = T{})
    {
        try
        {
            return std::any_cast<T&>(config.at(key));
        }
        catch (const std::out_of_range& /*e*/)
        {
            spdlog::warn("GenericConfig: Missing config item '{}', returning default", key);

            config[key] = default_value;
            return std::any_cast<T&>(config.at(key));
        }
    }

    template <class T>
    void addConfig(const std::string& key, const T& value)
    {
        config[key] = value;
    }

    template <class T>
    void addPerContextConfig(const std::string& ctx, const std::string& key, const T&& value)
    {
        config[ctx + "::" + key] = value;
    }
};
}} // namespace pg::game
