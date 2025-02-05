#pragma once
#include <sdlpp.hpp>
#include <any>

namespace pg {

class RendererState
{
public:
    virtual ~RendererState() = default;
    virtual void apply(sdl::Renderer& renderer) = 0;
    virtual void restore(sdl::Renderer& renderer) = 0;
};

class TextureState
{
public:
    virtual ~TextureState() = default;
    virtual void apply(sdl::Renderer& renderer, sdl::Texture& texture) = 0;
    virtual void restore(sdl::Renderer& renderer, sdl::Texture& texture) = 0;
};

class BlendModeState : public RendererState
{
public:
    BlendModeState(SDL_BlendMode blendMode)
      : blendMode(blendMode)
    {
    }

    void apply(sdl::Renderer& renderer) override
    {
        renderer.getDrawBlendMode(&prevBlendMode);
        renderer.setDrawBlendMode(blendMode);
    }

    void restore(sdl::Renderer& renderer) override { renderer.setDrawBlendMode(prevBlendMode); }

private:
    SDL_BlendMode       prevBlendMode;
    const SDL_BlendMode blendMode;
};

class ColorState : public RendererState
{
public:
    ColorState(const Color& color)
      : _color(color)
    {
    }

    void apply(sdl::Renderer& renderer) override
    {
        renderer.getDrawColor(&_prevColor[0], &_prevColor[1], &_prevColor[2], &_prevColor[3]);
        renderer.setDrawColor(_color[0], _color[1], _color[2], _color[3]);
    }

    void restore(sdl::Renderer& renderer) override
    {
        renderer.setDrawColor(_prevColor[0], _prevColor[1], _prevColor[2], _prevColor[3]);
    }

    auto getColor() const { return _color; }

    void setColor(const Color& color) { _color = color; }

public:
    Color _prevColor{};
    Color _color{};
};

class TextureColorState : public TextureState
{
public:
    TextureColorState(const Color& color)
      : color(color)
    {
    }

    void apply(sdl::Renderer&, sdl::Texture& texture) override
    {
        texture.getColorMod(&prevColor[0], &prevColor[1], &prevColor[2]);
        texture.setColorMod(color[0], color[1], color[2]);
    }

    void restore(sdl::Renderer&, sdl::Texture& texture) override
    {
        texture.setColorMod(prevColor[0], prevColor[1], prevColor[2]);
    }

private:
    Color       prevColor;
    const Color color;
};

class TextureBlendModeState : public TextureState
{
public:
    TextureBlendModeState(SDL_BlendMode blendMode)
      : blendMode(blendMode)
    {
    }

    void apply(sdl::Renderer&, sdl::Texture& texture) override
    {
        texture.getBlendMode(&storeBlendMode);
        texture.setBlendMode(blendMode);
    }

    void restore(sdl::Renderer&, sdl::Texture& texture) override { texture.setBlendMode(storeBlendMode); }

private:
    SDL_BlendMode       storeBlendMode;
    const SDL_BlendMode blendMode;
};

class TextureAlphaState : public TextureState
{
public:
    TextureAlphaState(uint8_t alpha)
      : _alpha(alpha)
    {
    }

    void apply(sdl::Renderer&, sdl::Texture& texture) override
    {
        texture.getAlphaMod(&_storeAlpha);
        texture.setAlphaMod(_alpha);
    }

    void restore(sdl::Renderer&, sdl::Texture& texture) override { texture.setAlphaMod(_storeAlpha); }

    auto getAlpha() const { return _alpha; }

    void setAlpha(uint8_t alpha) { _alpha = alpha; }

private:
    uint8_t _storeAlpha{};
    uint8_t _alpha;
};

class RendererDelegegateState : public RendererState
{
    using StoreFunc = std::function<void(sdl::Renderer&, std::any&)>;

public:
    RendererDelegegateState(StoreFunc&& store, StoreFunc&& restore)
      : storeFunc(store)
      , restoreFunc(restore)
    {
    }

    void apply(sdl::Renderer& renderer) override { storeFunc(renderer, storedState); };

    void restore(sdl::Renderer& renderer) override { restoreFunc(renderer, storedState); }

private:
    StoreFunc storeFunc;
    StoreFunc restoreFunc;

    std::any storedState;
};
} // namespace pg