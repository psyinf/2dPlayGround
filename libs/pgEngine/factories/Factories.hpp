#pragma once

#include <mutex>
#include <pgEngine/font/Font.hpp>
#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/primitives/FramedSprite.hpp>
#include <pgf/caching/ResourceLocator.hpp>
#include <sdlpp.hpp>

namespace pg {
// TODO: get from resource manager
// TODO: don't return sprites but textures

class SpriteFactory
{
public:
    // TODO: progress callback
    static auto loadFromFile(const std::string& path) -> SDL_Surface*;

    static auto loadFromBuffer(const std::vector<char>& buffer) -> SDL_Surface*;

    static sdl::Texture makeTexture(sdl::Renderer& renderer, std::string_view resource_name);
    static sdl::Texture makeTexture(sdl::Renderer& renderer, const std::vector<char>& buffer);

    static Sprite makeSprite(sdl::Renderer& renderer, std::string_view resource_name);
    static Sprite makeSprite(sdl::Renderer& renderer, const std::vector<char>& buffer);

    static Sprite makeTextSprite(sdl::Renderer& renderer, SDLFont& font, std::string_view text);
    /**
     * /brief
     * /param renderer
     * /param width num of horizontal frames
     * /param height num of vertical frames
     * /param resource_name
     * /return
     */
    static FramedSprite makeFramedSprite(
        sdl::Renderer&                         renderer,
        uint16_t                               width,
        uint16_t                               height,
        std::string_view                       resource_name,
        FramedSprite::FrameCalculationFunction frameFunction = FramedSprite::DefaultFrameCalculationFunction);

private:
    static inline pg::foundation::PathResourceLocator _resourceLocator{"../data"};
    static inline std::mutex                          _sdlMutex;
    static inline std::mutex                          _loaderMutex;
};

} // namespace pg
