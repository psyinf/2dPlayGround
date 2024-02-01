#include "Factories.hpp"

#include <SDL_image.h>
#include <font/Font.hpp>
#include "SDL_ttf.h"




pg::Sprite pg::SpriteFactory::makeSprite(sdl::Renderer& renderer, std::string_view resource_name)
{
    sdl::Surface spriteSurface(IMG_Load(resource_name.data()));
    sdl::Texture tex(renderer.get(), spriteSurface.get());
    return Sprite(std::move(tex));
}

pg::Sprite pg::SpriteFactory::makeTextSprite(sdl::Renderer& renderer, SDLFont& font, std::string_view text)
{
    static constexpr auto white = SDL_Color{255, 255, 255, 255};
    // TODO: get from resource manager

    auto surface = sdl::Surface(TTF_RenderText_Blended(font.get(), "Hello, World!", std::bit_cast<SDL_Color>(white)));
    return Sprite(sdl::Texture(renderer.get(), surface.get()));
}
