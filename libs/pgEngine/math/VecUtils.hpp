#pragma once
#include <pgEngine/math/Vec.hpp>
#include <sdlpp.hpp>

namespace pg {

template <typename Type>
constexpr Vec<Type, 2> dimsFromRect(const SDL_Rect& rect)
{
    return {static_cast<Type>(rect.w), static_cast<Type>(rect.h)};
}

template <typename Type>
constexpr Vec<Type, 2> startFromRect(const SDL_Rect& rect)
{
    return {static_cast<Type>(rect.x), static_cast<Type>(rect.y)};
}
} // namespace pg