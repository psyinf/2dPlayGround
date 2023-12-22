#pragma once
#include "SDLSprite.h"
#include <sdlpp.hpp>



namespace pg {

class SpriteFactory
{
public:
    static Sprite makeSprite(sdl::Renderer& renderer, std::string_view resource_name);

    static Sprite makeTextSprite(sdl::Renderer&   renderer,
                                 std::string_view text,
                                 std::string_view fontName,
                                 uint16_t         ptsize);
};

} // namespace pg