#pragma once
#include <any>
#include "sdlpp.hpp"
namespace pg {

class RendererState
{

public:
    virtual ~RendererState() = default;
    virtual void apply(sdl::Renderer& renderer) = 0;
    virtual void restore(sdl::Renderer& renderer) = 0;
};

class ColorState : public RendererState
{
    ColorState(const Color& color)
      : color(color)
    {
        
    }
    void apply(sdl::Renderer& renderer) override
    {
        pg::Color storeColor;
        renderer.getDrawColor(&storeColor[0], &storeColor[1], &storeColor[2], &storeColor[3]);
        renderer.setDrawColor(color[0], color[1], color[2], color[3]);
        color = storeColor;
    }

    void restore(sdl::Renderer& renderer) override {
        renderer.setDrawColor(color[0], color[1], color[2], color[3]);
    }

public:
    Color color;
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