#pragma once    
#include <sdlpp.hpp>
#include <SDLSprite.h>
#include <SDL_image.h>
namespace pg {

class SpriteFactory
{
public:
    static Sprite makeSprite(sdl::Renderer& renderer, std::string_view resource_name)
    {
        sdl::Surface spriteSurface(IMG_Load(resource_name.data()));
        sdl::Texture tex(renderer.get(), spriteSurface.get());
        return Sprite(std::move(tex));
    }
};

} // namespace pg