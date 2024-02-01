#pragma once
#include <pgEngine/core/State.hpp>

namespace pg {
using RendererStatePointer = std::unique_ptr<RendererState>;
using TextureStatePointer = std::unique_ptr<TextureState>;

class States
{
    template <class... T>
    static constexpr bool always_false = false;

public:
    States() = default;

    States(std::vector<RendererStatePointer>&& rendererState, std::vector<TextureStatePointer>&& textureState)
      : rendererStates{std::move(rendererState)}
      , textureStates{std::move(textureState)}
    {
    }

    void apply(sdl::Renderer& renderer) const
    {
        for (auto& state : rendererStates)
        {
            state->apply(renderer);
        }
    }

    void apply(sdl::Renderer& renderer, sdl::Texture& texture) const
    {
        for (auto& state : textureStates)
        {
            state->apply(renderer, texture);
        }
    }

    void restore(sdl::Renderer& renderer) const
    {
        for (auto& state : rendererStates)
        {
            state->restore(renderer);
        }
    }

    void restore(sdl::Renderer& renderer, sdl::Texture& texture) const
    {
        for (auto& state : textureStates)
        {
            state->restore(renderer, texture);
        }
    }

    template <typename T>
    constexpr void push(T&& state)
    {
        if constexpr (std::is_base_of_v<pg::RendererState, std::decay_t<T>>)
        {
            rendererStates.emplace_back(std::make_unique<std::decay_t<T>>(std::forward<T>(state)));
        }
        else if constexpr (std::is_base_of_v<pg::TextureState, std::decay_t<T>>)
        {
            textureStates.emplace_back(std::make_unique<std::decay_t<T>>(std::forward<T>(state)));
        }
        else { static_assert(always_false<T>, "Invalid state type"); }
    }

private:
    std::vector<RendererStatePointer> rendererStates;
    std::vector<TextureStatePointer>  textureStates;
};
} // namespace pg