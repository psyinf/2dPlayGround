#include "Factories.hpp"

#include <SDL_image.h>
#include <font/Font.hpp>
#include <pgf/caching/ResourceLocator.hpp>
#include "SDL_ttf.h"

#include <spdlog/spdlog.h>

// TODO: progress callback
static auto loadFromFile(const std::string& path)
{
    static std::mutex m;
    // get the file into a std::vector<char>
    std::ifstream     file(path, std::ios::binary);
    std::vector<char> buff((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::lock_guard<std::mutex> lock(m);
    SDL_RWops*                  rw = SDL_RWFromMem(buff.data(), buff.size());
    auto                        img = IMG_Load_RW(rw, 1);
    return img;
}

sdl::Texture pg::SpriteFactory::makeTexture(sdl::Renderer& renderer, std::string_view resource_name)
{
    auto img = loadFromFile(resource_name.data());
    if (!img)
    {
        spdlog::error("Failed to load image: {} {}", resource_name, IMG_GetError());
        sdl::Surface default_surface(SDL_CreateRGBSurface(0, 1, 1, 8, 0, 0, 0, 0));
        return sdl::Texture(renderer.get(), default_surface.get());
    }
    sdl::Surface surface(IMG_Load(resource_name.data()));
    return sdl::Texture(renderer.get(), surface.get());
}

pg::Sprite pg::SpriteFactory::makeSprite(sdl::Renderer& renderer, std::string_view resource_name)
{
    const auto path = _resourceLocator.locate(std::string{resource_name});
    auto       img = loadFromFile(path.string());
    if (!img)
    {
        spdlog::error("Failed to load image: {} {}", resource_name, IMG_GetError());
        sdl::Surface default_surface(SDL_CreateRGBSurface(0, 1, 1, 8, 0, 0, 0, 0));
        return Sprite(std::make_shared<sdl::Texture>(renderer.get(), default_surface.get()));
    }
    // TODO: use IMG_loadTexture
    sdl::Surface spriteSurface(img);
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
