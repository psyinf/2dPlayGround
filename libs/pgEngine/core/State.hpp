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
      : _blendMode(blendMode)
    {
    }

    void apply(sdl::Renderer& renderer) override
    {
        renderer.getDrawBlendMode(&_prevBlendMode);
        renderer.setDrawBlendMode(_blendMode);
    }

    void restore(sdl::Renderer& renderer) override { renderer.setDrawBlendMode(_prevBlendMode); }

private:
    SDL_BlendMode       _prevBlendMode{};
    const SDL_BlendMode _blendMode;
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
      : _color(color)
    {
    }

    void apply(sdl::Renderer&, sdl::Texture& texture) override
    {
        texture.getColorMod(&_prevColor[0], &_prevColor[1], &_prevColor[2]);
        texture.setColorMod(_color[0], _color[1], _color[2]);
    }

    void restore(sdl::Renderer&, sdl::Texture& texture) override
    {
        texture.setColorMod(_prevColor[0], _prevColor[1], _prevColor[2]);
    }

private:
    Color       _prevColor;
    const Color _color;
};

class TextureBlendModeState : public TextureState
{
public:
    TextureBlendModeState(SDL_BlendMode blendMode)
      : _blendMode(blendMode)
    {
    }

    void apply(sdl::Renderer&, sdl::Texture& texture) override
    {
        texture.getBlendMode(&_storeBlendMode);
        texture.setBlendMode(_blendMode);
    }

    void restore(sdl::Renderer&, sdl::Texture& texture) override { texture.setBlendMode(_storeBlendMode); }

private:
    SDL_BlendMode       _storeBlendMode;
    const SDL_BlendMode _blendMode;
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

    void apply(sdl::Renderer& renderer) override { storeFunc(renderer, storedState); }

    void restore(sdl::Renderer& renderer) override { restoreFunc(renderer, storedState); }

private:
    StoreFunc storeFunc;
    StoreFunc restoreFunc;

    std::any storedState;
};
} // namespace pg
