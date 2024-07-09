#pragma once
#include <pgFoundation/caching/ResourceManager.hpp>
#include <pgEngine/factories/Factories.hpp>

namespace pg::foundation {

template <>
inline pg::Sprite loadResource<pg::Sprite, sdl::Renderer&>(const std::string& path, sdl::Renderer& r)
{
    return pg::SpriteFactory::makeSprite(r, path);
}

template <>
inline sdl::Texture loadResource<sdl::Texture>(const std::string& path, sdl::Renderer& r)
{
    return pg::SpriteFactory::makeTexture(r, path);
}

} // namespace pg::foundation
