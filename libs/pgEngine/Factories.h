#pragma once
#include <SDLSprite.h>
#include <sdlpp.hpp>

#include <SDL_image.h>
#include <SDL_ttf.h>

namespace pg {

static constexpr auto deleter = [](TTF_Font* p) { TTF_CloseFont(p); };
using FontPtr = std::unique_ptr<TTF_Font, decltype(deleter)>;

class SpriteFactory
{
public:
    static Sprite makeSprite(sdl::Renderer& renderer, std::string_view resource_name)
    {
        sdl::Surface spriteSurface(IMG_Load(resource_name.data()));
        sdl::Texture tex(renderer.get(), spriteSurface.get());
        return Sprite(std::move(tex));
    }

    static Sprite makeTextSprite(sdl::Renderer&   renderer,
                                 std::string_view text,
                                 std::string_view fontName,
                                 uint16_t         ptsize)
    {
        static constexpr auto white = SDL_Color{255, 255, 255, 255};
        // TODO: get from resource manager
        auto font = FontPtr(TTF_OpenFont(fontName.data(), ptsize));
        if (!font) { throw std::invalid_argument(TTF_GetError()); }
        auto surface = std::make_unique<sdl::Surface>(
            TTF_RenderText_Blended(font.get(), "Hello, World!", std::bit_cast<SDL_Color>(white)));
        return Sprite(sdl::Texture(renderer.get(), surface.get()->get()));
   
    }
};

} // namespace pg