#pragma once
#include <pgEngine/font/Font.hpp>
#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/primitives/FramedSprite.hpp>
#include <pgFoundation/caching/ResourceLocator.hpp>
#include <sdlpp.hpp>

namespace pg {
// TODO: get from resource manager
// TODO: don't return sprites but textures

class SpriteFactory
{
public:
    static sdl::Texture makeTexture(sdl::Renderer& renderer, std::string_view resource_name);

    static Sprite makeSprite(sdl::Renderer& renderer, std::string_view resource_name);

    static Sprite makeTextSprite(sdl::Renderer& renderer, SDLFont& font, std::string_view text);
    /**
     * @brief
     * @param renderer
     * @param width num of horizontal frames
     * @param height num of vertical frames
     * @param resource_name
     * @return
     */
    static FramedSprite makeFramedSprite(sdl::Renderer&   renderer,
                                         uint16_t         width,
                                         uint16_t         height,
                                         std::string_view resource_name);

private:
    static inline pg::foundation::PathResourceLocator _resourceLocator{"../data"};
};

} // namespace pg
