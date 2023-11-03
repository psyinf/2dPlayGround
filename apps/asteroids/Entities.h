#pragma once
#include <SDLVec.h>
#include <SDLPrimitives.h>
#include <memory>
#include <chrono>
namespace game {

struct FrameStamp
{
    std::uint64_t             frameNumber;
    std::chrono::milliseconds lastFrameDuration;
};

struct Drawable
{
    // TODO: get around polymorphism
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
