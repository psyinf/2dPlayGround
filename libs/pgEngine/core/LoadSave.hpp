#pragma once
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <exception>

#include <spdlog/spdlog.h>

namespace pg {
template <typename T>
static inline T load(const std::filesystem::path& file_name)
{
    try
    {
        std::ifstream file(file_name.c_str(), std::ios_base::binary);
        if (!file.is_open()) { throw std::runtime_error("Failed to open file " + file_name.string()); }
        nlohmann::json j;
        file >> j;
        return j.get<T>();
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Failed to load file " + file_name.string() + " : " + e.what());
    }
}

template <typename T>
static inline T load(const std::filesystem::path& file_name, const T& default_value)
{
    try
    {
        std::ifstream file(file_name.c_str(), std::ios_base::binary);
        if (!file.is_open()) { throw std::runtime_error("Failed to open file " + file_name.string()); }
        nlohmann::json j;
        file >> j;
        return j.get<T>();
    }
    catch (const std::exception& e)
    {
        spdlog::warn("Failed to load file {} : {}, resorting to default", file_name.string(), e.what());
        // throw std::runtime_error("Failed to load file " + file_name.string() + " : " + e.what());
        return default_value;
    }
}

template <typename T>
static inline void save(std::string_view file_name, const T& type)
{
    try
    {
        std::ofstream file(file_name.data(), std::ios_base::binary);
        if (!file.is_open()) { throw std::runtime_error("Failed to open file " + std::string(file_name)); }
        nlohmann::json j = type;
        file << j.dump(4);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Failed to save file " + std::string(file_name) + " : " + e.what());
    }
}
} // namespace pg