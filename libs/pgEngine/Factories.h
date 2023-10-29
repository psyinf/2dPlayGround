#pragma once    
#include <sdlpp.hpp>

namespace pg {

class SpriteFactory
{
public:
    static std::unique_ptr<Sprite> makeSprite(sdl::Renderer& renderer, std::string_view resource_name)
    {
        sdl::Surface spriteSurface(IMG_Load(resource_name.data()));
        sdl::Texture tex(renderer.get(), spriteSurface.get());
        return std::make_unique<Sprite>(std::move(tex));
    }
};

} // namespace pg