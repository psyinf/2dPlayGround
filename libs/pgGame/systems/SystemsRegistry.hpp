#pragma once
#include <foundation/caching/GenericFactory.hpp>

namespace pg::game {
class Game;

class SystemsFactory
{
    using Factory = foundation::GenericFactory<pg::game::SystemInterface, pg::game::Game&>;

public:
    template <typename T, typename... Args>
    static void registerSystem(std::string name)
    {
        factory.registerPrototype(name, foundation::GenericFactory<T, pg::game::Game&, Args...>::proto());
    }

    static auto makeSystem(std::string_view key, pg::game::Game& game) { return factory.make(key, game); }

    static auto& getFactory() { return factory; }

private:
    static inline Factory factory;
};
} // namespace pg::game