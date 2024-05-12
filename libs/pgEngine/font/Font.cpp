#include "font/Font.hpp"

// #include<SDL_ttf.h>

pg::SDLFont::SDLFont(const std::string_view path, int size)
{
    //  font = TTF_OpenFont(path.data(), size);
}

pg::SDLFont::~SDLFont()
{
    //  TTF_CloseFont(font);
}

auto pg::SDLFont::get() -> TTF_Font*
{
    return font;
}

auto pg::SDLFont::get() const -> const TTF_Font*
{
    return font;
}
