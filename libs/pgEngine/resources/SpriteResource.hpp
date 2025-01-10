#pragma once
#include <pgf/caching/ResourceManager.hpp>
#include <pgEngine/factories/Factories.hpp>

namespace pg::foundation {

template <>
inline pg::Sprite loadResource<pg::Sprite, sdl::Renderer&>(DataProvider& p, sdl::Renderer& r)
{
    return pg::SpriteFactory::makeSprite(r, p.getUri().uri);
}

template <>
inline sdl::Texture loadResource<sdl::Texture>(DataProvider& p, sdl::Renderer& r)
{
    return pg::SpriteFactory::makeTexture(r, p.getUri().uri);
}

} // namespace pg::foundation
