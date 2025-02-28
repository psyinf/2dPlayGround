#include "Factories.hpp"
#include <fstream>
#include <SDL_image.h>
#include <font/Font.hpp>
#include <pgf/caching/ResourceLocator.hpp>
#include "SDL_ttf.h"

#include <spdlog/spdlog.h>

auto pg::SpriteFactory::loadFromFile(const std::string& path) -> SDL_Surface*
{
    // get the file into a std::vector<char>
    std::ifstream file(path, std::ios::binary);
    return loadFromBuffer(std::vector<char>(std::istreambuf_iterator<char>(file), {}));
}

auto pg::SpriteFactory::loadFromBuffer(const std::vector<char>& buffer) -> SDL_Surface*
{
    std::lock_guard<std::mutex> lock(_loaderMutex);
    SDL_RWops*                  rw = SDL_RWFromMem((void*)buffer.data(), static_cast<int>(buffer.size()));
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
    // TODO: we cannot do this in parallel to rendering as it is not thread safe
    std::lock_guard<std::mutex> lock(_sdlMutex);
    sdl::Surface                surface(img);
    return sdl::Texture(renderer.get(), surface.get());
}

sdl::Texture pg::SpriteFactory::makeTexture(sdl::Renderer& renderer, const std::vector<char>& buffer)
{
    auto img = loadFromBuffer(buffer);

    if (!img)
    {
        spdlog::error("Failed to get image from buffer {}", IMG_GetError());
        sdl::Surface default_surface(SDL_CreateRGBSurface(0, 1, 1, 8, 0, 0, 0, 0));
        return sdl::Texture(renderer.get(), default_surface.get());
    }
    // TODO: we cannot do this in parallel to rendering as it is not thread safe
    std::lock_guard<std::mutex> lock(_sdlMutex);
    sdl::Surface                surface(img);
    return sdl::Texture(renderer.get(), surface.get());
}

pg::Sprite pg::SpriteFactory::makeSprite(sdl::Renderer& renderer, const std::vector<char>& data)
{
    auto img = loadFromBuffer(data);

    if (!img)
    {
        spdlog::error("Failed to get image from buffer {}", IMG_GetError());
        sdl::Surface default_surface(SDL_CreateRGBSurface(0, 1, 1, 8, 0, 0, 0, 0));
        return Sprite(std::make_shared<sdl::Texture>(renderer.get(), default_surface.get()));
    }
    std::lock_guard<std::mutex> lock(_sdlMutex);
    sdl::Surface                spriteSurface(img);
    return Sprite(std::make_shared<sdl::Texture>(renderer.get(), spriteSurface.get()));
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
    std::lock_guard<std::mutex> lock(_sdlMutex);
    sdl::Surface                spriteSurface(img);
    return Sprite(std::make_shared<sdl::Texture>(renderer.get(), spriteSurface.get()));
}

pg::Sprite pg::SpriteFactory::makeTextSprite(sdl::Renderer& renderer, SDLFont& font, std::string_view text)
{
    std::lock_guard<std::mutex> lock(_sdlMutex);
    auto surface = sdl::Surface(TTF_RenderText_Blended(font.get(), text.data(), std::bit_cast<SDL_Color>(white)));
    return Sprite(std::make_shared<sdl::Texture>(renderer.get(), surface.get()));
}

pg::FramedSprite pg::SpriteFactory::makeFramedSprite(sdl::Renderer&                         renderer,
                                                     uint16_t                               width,
                                                     uint16_t                               height,
                                                     std::string_view                       resource_name,
                                                     FramedSprite::FrameCalculationFunction frameFunction)
{
    sdl::Surface                surface(loadFromFile(resource_name.data()));
    std::lock_guard<std::mutex> lock(_sdlMutex);

    return FramedSprite(std::make_shared<sdl::Texture>(renderer.get(), surface.get()), width, height, frameFunction);
}
