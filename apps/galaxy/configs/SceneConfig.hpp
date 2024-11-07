#pragma once

namespace galaxy {

struct SceneConfig
{
    SceneSoundScape soundScape;
};

//a configuration map holding all configurations 
//  current idea: each system will fill the map with its own configuration
//
struct MasterConfig {
    std::unordered_map<std::string, std::any> config;
    template <class T>
    T& getPerSceneConfig(const std::string scene, const std::string& key)
    {
        return std::any_cast<T&>(config[key]);
    }
};

} // namespace galaxy
