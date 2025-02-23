#pragma once
#include <pgf/caching/GenericFactory.hpp>
#include <string>

namespace pg::game {
class Game;
using namespace std::string_view_literals;

class SystemsFactory
{
    using Factory = foundation::GenericFactory<pg::game::SystemInterface, pg::game::Game&, const std::string&>;
    using SharedSystem = std::shared_ptr<pg::game::SystemInterface>;
    using SharedSystems = std::unordered_map<std::string, SharedSystem>;

    static constexpr auto sharedSystemPrefix = "::"sv;

public:
    template <typename T, typename... Args>
    static void registerSystem(const std::string& name, Args&&... args)
    {
        _factory.registerPrototype(
            name,
            foundation::GenericFactory<T, pg::game::Game&, const std::string&>::proto(std::forward<Args>(args)...));
    }

    static auto makeSystem(std::string_view key, pg::game::Game& game) -> std::shared_ptr<pg::game::SystemInterface>
    {
        // check if a shared system is requested
        if (key.starts_with(sharedSystemPrefix))
        {
            auto sharedKey = std::string{key.substr(sharedSystemPrefix.size())};
            if (!_sharedSystems.contains(sharedKey))
            {
                _sharedSystems.emplace(sharedKey, _factory.make(sharedKey, game, std::string{sharedKey}));
            }
            return _sharedSystems.at(sharedKey);
        }

        return _factory.make(key, game, std::string{key});
    }

    static auto& getFactory() { return _factory; }

    auto getRegisteredSystems() const { return _factory.getKeys(); }

private:
    static inline Factory       _factory;
    static inline SharedSystems _sharedSystems;
};
} // namespace pg::game
