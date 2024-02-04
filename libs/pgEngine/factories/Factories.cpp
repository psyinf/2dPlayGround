#include "Factories.hpp"

#include <SDL_image.h>
#include <font/Font.hpp>
#include "SDL_ttf.h"

pg::Sprite pg::SpriteFactory::makeSprite(sdl::Renderer& renderer, std::string_view resource_name)
{
    sdl::Surface spriteSurface(IMG_Load(resource_name.data()));
    return Sprite(sdl::Texture(renderer.get(), spriteSurface.get()));
}

pg::Sprite pg::SpriteFactory::makeTextSprite(sdl::Renderer& renderer, SDLFont& font, std::string_view text)
{
    static constexpr auto white = SDL_Color{255, 255, 255, 255};
    auto surface = sdl::Surface(TTF_RenderText_Blended(font.get(), text.data(), std::bit_cast<SDL_Color>(white)));
    return Sprite(sdl::Texture(renderer.get(), surface.get()));
}

pg::FramedSprite pg::SpriteFactory::makeFramedSprite(sdl::Renderer&   renderer,
                                                     uint16_t         width,
                                                     uint16_t         height,
                                                     std::string_view resource_name)
{
    sdl::Surface surface(IMG_Load(resource_name.data()));
    return FramedSprite(sdl::Texture(renderer.get(), surface.get()), width, height);
}
