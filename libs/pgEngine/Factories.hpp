#pragma once
#include "SDLFont.hpp"
#include "SDLSprite.h"
#include <sdlpp.hpp>

namespace pg {

class SpriteFactory
{
public:
    static Sprite makeSprite(sdl::Renderer& renderer, std::string_view resource_name);

    static Sprite makeTextSprite(sdl::Renderer& renderer, SDLFont& font, std::string_view text );
};

} // namespace pg