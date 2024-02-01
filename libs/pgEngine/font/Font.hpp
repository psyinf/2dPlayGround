#pragma once
#include <string>

typedef struct _TTF_Font TTF_Font;

namespace pg {

class SDLFont
{
public:
    SDLFont(const std::string_view path, int size);
    ~SDLFont();

    auto get() -> TTF_Font*;

    auto get() const -> const TTF_Font*;

private:
    TTF_Font* font{};
};
} // namespace pg