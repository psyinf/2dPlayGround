#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgGame/components/Drawable.hpp>

namespace sdl {
class Renderer;
}

namespace pg {
class States;
}

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

template <typename... Tags>
class TaggedRenderSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup();

    void handle(const pg::game::FrameStamp& frameStamp);

    template <typename... Args, typename... Excludes>
    auto getView(Excludes... excludes)
    {
        return game.getGlobalRegistry().view<Args..., Tags...>(std::forward<Excludes>(excludes)...);
    }
};

using RenderSystem = TaggedRenderSystem<>;

} // namespace galaxy