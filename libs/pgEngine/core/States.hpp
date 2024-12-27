#pragma once
#include <pgEngine/core/State.hpp>

namespace pg {
using RendererStatePointer = std::shared_ptr<RendererState>;
using TextureStatePointer = std::shared_ptr<TextureState>;

class States
{
    template <class... T>
    static constexpr bool always_false = false;

public:
    States() = default;

    States(States&&) = default;

    States& operator=(States&&) = default;

    States(const States&) = default;

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

    void push_states(const States& states)
    {
        rendererStates.insert(rendererStates.end(), states.rendererStates.begin(), states.rendererStates.end());
        textureStates.insert(textureStates.end(), states.textureStates.begin(), states.textureStates.end());
    }

    void pop_states(const States& states)
    {
        for ([[maybe_unused]] const auto& _ : states.rendererStates)
        {
            rendererStates.pop_back();
        }
        for ([[maybe_unused]] const auto& _ : states.textureStates)
        {
            textureStates.pop_back();
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

    template <typename T>
    constexpr void pop()
    {
        if constexpr (std::is_base_of_v<pg::RendererState, std::decay_t<T>>) { rendererStates.pop_back(); }
        else if constexpr (std::is_base_of_v<pg::TextureState, std::decay_t<T>>) { textureStates.pop_back(); }
        else { static_assert(always_false<T>, "Invalid state type"); }
    }

    // get a specific state
    // caveat: will return the first state of the type found
    template <typename T>
    constexpr std::shared_ptr<T> get()
    {
        if constexpr (std::is_base_of_v<pg::RendererState, T>)
        {
            for (auto& state : rendererStates)
            {
                if (auto s = std::dynamic_pointer_cast<T>(state); s != nullptr) { return s; }
            }
        }
        else if constexpr (std::is_base_of_v<pg::TextureState, T>)
        {
            for (auto& state : textureStates)
            {
                if (auto s = std::dynamic_pointer_cast<T>(state); s != nullptr) { return s; }
            }
        }
        return nullptr;
    }

    template <typename T>
    void replace(T&& state)
    {
        if constexpr (std::is_base_of_v<pg::RendererState, std::decay_t<T>>)
        {
            for (auto& s : rendererStates)
            {
                if (auto r = std::dynamic_pointer_cast<T>(s); r != nullptr)
                {
                    s = std::make_unique<std::decay_t<T>>(std::forward<T>(state));
                    return;
                }
            }
        }
        else if constexpr (std::is_base_of_v<pg::TextureState, std::decay_t<T>>)
        {
            for (auto& s : textureStates)
            {
                if (auto r = std::dynamic_pointer_cast<T>(s); r != nullptr)
                {
                    s = std::make_unique<std::decay_t<T>>(std::forward<T>(state));
                    return;
                }
            }
        }
    }

private:
    std::vector<RendererStatePointer> rendererStates;
    std::vector<TextureStatePointer>  textureStates;
};

} // namespace pg
