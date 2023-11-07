#pragma once
#include <SDLVec.h>
#include <SDLPrimitives.h>
#include <memory>

namespace game {



struct Drawable
{
    // TODO: get around dynamic polymorphism maybe using CRTP
    std::unique_ptr<pg::Primitive> prim;
};

struct Dynamics
{
    pg::fVec2 velocity{};
    pg::fVec2 dampening{0.9, 0.9};
};

struct WindowDetails
{
    SDL_Rect windowRect;
};
} // namespace game
