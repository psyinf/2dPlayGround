#include "Factories.hpp"

#include <SDL_image.h>
#include <font/Font.hpp>
#include <pgf/caching/ResourceLocator.hpp>
#include "SDL_ttf.h"

sdl::Texture pg::SpriteFactory::makeTexture(sdl::Renderer& renderer, std::string_view resource_name)
{
    sdl::Surface surface(IMG_Load(resource_name.data()));
    return sdl::Texture(renderer.get(), surface.get());
}

pg::Sprite pg::SpriteFactory::makeSprite(sdl::Renderer& renderer, std::string_view resource_name)
{
    const auto   path = _resourceLocator.locate(std::string{resource_name});
    sdl::Surface spriteSurface(IMG_Load(path.string().c_str()));
    return Sprite(std::make_shared<sdl::Texture>(renderer.get(), spriteSurface.get()));
}

pg::Sprite pg::SpriteFactory::makeTextSprite(sdl::Renderer& renderer, SDLFont& font, std::string_view text)
{
    auto surface = sdl::Surface(TTF_RenderText_Blended(font.get(), text.data(), std::bit_cast<SDL_Color>(white)));
    return Sprite(std::make_shared<sdl::Texture>(renderer.get(), surface.get()));
}

pg::FramedSprite pg::SpriteFactory::makeFramedSprite(sdl::Renderer&                         renderer,
                                                     uint16_t                               width,
                                                     uint16_t                               height,
                                                     std::string_view                       resource_name,
                                                     FramedSprite::FrameCalculationFunction frameFunction)
{
    sdl::Surface surface(IMG_Load(resource_name.data()));
    return FramedSprite(std::make_shared<sdl::Texture>(renderer.get(), surface.get()), width, height, frameFunction);
}
